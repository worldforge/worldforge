/*
 Copyright (C) 2022 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "CLI.h"
#include "squall/core/Repository.h"
#include "squall/core/Generator.h"
#include "squall/core/Resolver.h"
#include "squall/curl/CurlProvider.h"
#include "squall/core/Realizer.h"
#include "squall/core/Log.h"
#include "squall/core/Pruner.h"

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include <spdlog/spdlog.h>
#include "bytesize/bytesize.hh"

using namespace Squall;

int main(int argc, char** argv) {
	setupEncodings();
	Squall::logger = spdlog::default_logger();
	CLI::App app{"Squall is a tool for syncing shared read only data from multiple sources."};

	std::string repositoryPath;
	app.add_option("-r,--repository", repositoryPath, "Location of the repository.")->required(true)->envname("SQUALL_REPOSITORY");
	app.add_flag("-v,--verbose", [](std::int64_t verbose) {
		spdlog::set_level(spdlog::level::trace);
		logger->trace("Verbose logging enabled");
	}, "Enable verbose logging.");


	{
		auto sourcePath = std::make_shared<std::string>();
		auto excludes = std::make_shared<std::vector<std::regex>>();
		auto includes = std::make_shared<std::vector<std::regex>>();
		auto generate = app.add_subcommand("generate", "Generate digests from an existing file structure.");
		generate->add_option("-s,--source", *sourcePath, "Location of source.")->required(true);
		generate->add_option("-e,--exclude", *excludes, "Regexp of paths to exclude.");
		generate->add_option("-i,--include", *includes, "Regexp of paths to include.");
		generate->final_callback([&repositoryPath, sourcePath, excludes, includes]() {
			Repository repository(repositoryPath);
			Generator generator(repository, *sourcePath, {.exclude = *excludes, .include=*includes});

			size_t filesProcessed = 0;
			GenerateResult result;
			do {
				result = generator.process(10);
				filesProcessed += result.processedFiles.size();
			} while (!result.complete);

			logger->info("Processed {} files.", filesProcessed);
			if (result.complete) {
				//The last processed file contains the signature for the whole directory.
				auto& lastEntry = *(--result.processedFiles.end());
				logger->info("Signature generated: {}", lastEntry.fileEntry.signature.str_view());
			} else {
				logger->error("Could not fully generate digests.");
			}
		});
	}
	{
		auto root = app.add_subcommand("root", "Handle named roots.");
		{
			auto add = root->add_subcommand("add", "Adds a new root.");
			auto rootName = std::make_shared<std::string>();
			auto signature = std::make_shared<std::string>();
			add->add_option("--root", *rootName, "Name of the root.")->required(true);
			add->add_option("--signature", *signature, "A signature.")->required(true);
			add->final_callback([&repositoryPath, rootName, signature]() {
				Repository repository(repositoryPath);
				repository.storeRoot(*rootName, Root{.signature = Signature(*signature)});

				logger->info("Root '{}' registered with signature '{}'.", *rootName, *signature);
			});
		}
		{
			auto add = root->add_subcommand("remove", "Removes a root.");
			auto rootName = std::make_shared<std::string>();
			auto signature = std::make_shared<std::string>();
			add->add_option("--root", *rootName, "Name of the root.")->required(true);
			add->final_callback([&repositoryPath, rootName, signature]() {
				Repository repository(repositoryPath);
				repository.removeRoot(*rootName);

				logger->info("Root '{}' has been removed.", *rootName, *signature);
			});
		}
	}


	{
		auto remotePath = std::make_shared<std::string>();
		auto rootName = std::make_shared<std::string>();
		auto signature = std::make_shared<std::string>();
		auto download = app.add_subcommand("download", "Download remote digest into the repository.");
		download->add_option("-r,--remote", *remotePath, "Location of remote as an URL.")->required(true);
		download->add_option("--root", *rootName, "Name of the root.");
		download->add_option("--signature", *signature, "A signature.");
		download->final_callback([&repositoryPath, remotePath, rootName, signature]() {
			Repository repository(repositoryPath);

			Signature signatureInstance{};
			if (!signature->empty()) {
				signatureInstance = Signature(*signature);
			} else {
				auto rootResult = repository.readRoot(*rootName);
				if (!rootResult) {
					logger->error("Could not find root with name '{}'.", *rootName);
				}
				signatureInstance = rootResult->signature;
			}

			if (!signatureInstance.isValid()) {
				throw std::runtime_error("Signature is not valid.");
			}


			logger->info("Downloading from '{}', starting at manifest '{}'.", *remotePath, signatureInstance.str_view());
			Resolver resolver(repository,
							  std::make_unique<CurlProvider>(*remotePath),
							  signatureInstance);
			ResolveResult result{};
			std::vector<ResolveEntry> downloadedFiles;
			do {
				result = resolver.poll(0);
				for (auto& entry: result.completedRequests) {
					downloadedFiles.emplace_back(entry);
				}
			} while (result.status == Squall::ResolveStatus::ONGOING);

			if (result.status == Squall::ResolveStatus::HAD_ERROR) {
				logger->error("Could not complete remote download.");
			} else {
				logger->info("Downloaded {} files.", downloadedFiles.size());
			}
		});
	}

	{
		auto directoryPath = std::make_shared<std::string>();
		auto rootName = std::make_shared<std::string>();
		auto signature = std::make_shared<std::string>();
		auto realizeConfig = std::make_shared<RealizerConfig>();
		auto realize = app.add_subcommand("realize", "Realize files onto the file system.");
		realize->add_option("-d,--destination", *directoryPath, "Filesystem path where files should be realized.")->required(true);
		realize->add_option("--root", *rootName, "Name of the root.");
		realize->add_option("--signature", *signature, "A signature.");
		std::map<std::string, RealizeMethod> map{{"copy",     RealizeMethod::COPY},
												 {"symlink",  RealizeMethod::SYMLINK},
												 {"hardlink", RealizeMethod::HARDLINK}};
		realize->add_option("--method", realizeConfig->method, "Method of realization.")
				->default_val(RealizeMethod::COPY)
				->transform(CLI::CheckedTransformer(map, CLI::ignore_case));


		realize->final_callback([&repositoryPath, directoryPath, rootName, signature, realizeConfig]() {
			Repository repository(repositoryPath);

			Signature signatureInstance{};
			if (!signature->empty()) {
				signatureInstance = Signature(*signature);
			} else {
				auto rootResult = repository.readRoot(*rootName);
				if (!rootResult) {
					logger->error("Could not find root with name {}.", *rootName);
				}
				signatureInstance = rootResult->signature;
			}

			if (!signatureInstance.isValid()) {
				throw std::runtime_error("Signature is not valid.");
			}

			auto manifestResult = repository.fetchManifest(signatureInstance);
			if (manifestResult.fetchResult.status == Squall::FetchStatus::FAILURE || !manifestResult.manifest.has_value()) {
				throw std::runtime_error("Could not fetch manifest from local repository.");
			}
			logger->info("Realizing manifest '{}' into '{}'.", signatureInstance.str_view(), *directoryPath);

			Squall::Iterator iterator(repository, *manifestResult.manifest);

			Realizer realizer(repository, *directoryPath, iterator, *realizeConfig);

			RealizeResult result{};
			do {
				result = realizer.poll();
			} while (result.status == Squall::RealizeStatus::INPROGRESS);

			if (result.status == Squall::RealizeStatus::INCOMPLETE) {
				logger->error("Could not complete realization.");
			} else {
				logger->info("Completed realizing files into '{}'.", *directoryPath);
			}
		});
	}

	{
		auto directoryPath = std::make_shared<std::string>();
		auto isDryRun = std::make_shared<bool>(false);
		auto prune = app.add_subcommand("prune", "Remove data that's not reached by any root.");
		prune->add_flag("--dry-run", *isDryRun, "Do not perform any deletion, just print result.");

		prune->final_callback([&repositoryPath, directoryPath, isDryRun]() {
			Repository repository(repositoryPath);
			auto abandonedFiles = Pruner::findAbandonedFiles(repository);
			if (*isDryRun) {
				uintmax_t totalSize = 0;
				for (auto& entry: abandonedFiles) {
					logger->info("Will delete {}", entry.generic_string());
					totalSize += file_size(entry);
				}
				logger->info("This would delete {} files, for a total of {} bytes.", abandonedFiles.size(), bytesize::bytesize(totalSize));
			} else {
				logger->info("Will now delete {} files.", abandonedFiles.size());
				uintmax_t totalSize = 0;
				for (auto& entry: abandonedFiles) {
					//Since we'll be deleting files we'll make extra sure there's no strangeness. This should under normal cases never happen.
					auto relative = std::filesystem::relative(entry, repository.getPath());
					if (relative.empty()) {
						logger->error("Entry {} is not in a subdirectory of {}, aborting.", entry.generic_string(), repository.getPath().generic_string());
						std::exit(1);
					} else {

						totalSize += file_size(entry);
						std::error_code ec;
						remove(entry, ec);
						if (ec) {
							logger->warn("Error when trying to delete file {}: {}", entry.generic_string(), ec.message());
						} else {
							logger->trace("Deleted {}", entry.generic_string());
							//Check if we also should remove the directory if it's now empty.
							if (std::filesystem::is_directory(entry.parent_path()) && is_empty(entry.parent_path())) {
								remove(entry.parent_path(), ec);
								if (ec) {
									logger->warn("Error when trying to remove now empty directory {}: {}", entry.parent_path().generic_string(), ec.message());
								} else {
									logger->trace("Removed now empty directory {}.", entry.parent_path().generic_string());
								}
							}
						}
					}
				}
				logger->info("Deleted {} files, for a total of {} bytes.", abandonedFiles.size(), bytesize::bytesize(totalSize));
			}

		});
	}
	CLI11_PARSE(app, argc, argv)
	return 0;
}