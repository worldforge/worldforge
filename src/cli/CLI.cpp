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

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>
#include <spdlog/spdlog.h>

using namespace Squall;

int main(int argc, char** argv) {
	setupEncodings();
	CLI::App app{"Squall is a tool for syncing shared read only data from multiple sources."};

	std::string repositoryPath;
	app.add_option("-r,--repository", repositoryPath, "Location of the repository.");

	{
		auto sourcePath = std::make_shared<std::string>();
		auto generate = app.add_subcommand("generate", "Generate digests from an existing file structure.");
		generate->add_option("-s,--source", *sourcePath, "Location of source.")->required(true);
		generate->final_callback([&repositoryPath, sourcePath]() {
			Repository repository(repositoryPath);
			Generator generator(repository, *sourcePath);

			size_t filesProcessed = 0;
			GenerateResult result;
			do {
				result = generator.process(10);
				filesProcessed += result.processedFiles.size();
			} while (!result.complete);

			spdlog::info("Processed {} files.", filesProcessed);
			if (result.complete) {
				//The last processed file contains the signature for the whole directory.
				auto& lastEntry = *(--result.processedFiles.end());
				spdlog::info("Signature generated: {}", lastEntry.fileEntry.signature.str_view());
			} else {
				spdlog::error("Could not fully generate digests.");
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

				spdlog::info("Root '{}' registered with signature '{}'.", *rootName, *signature);
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

				spdlog::info("Root '{}' has been removed.", *rootName, *signature);
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
					spdlog::error("Could not find root with name '{}'.", *rootName);
				}
				signatureInstance = rootResult->signature;
			}

			if (!signatureInstance.isValid()) {
				throw std::runtime_error("Signature is not valid.");
			}


			spdlog::info("Downloading from '{}', starting at manifest '{}'.", *remotePath, signatureInstance.str_view());
			Resolver resolver(repository,
							  std::make_unique<CurlProvider>(*remotePath),
							  signatureInstance);
			ResolveResult result{};
			size_t downloadedFiles = 0;
			do {
				result = resolver.poll();
				downloadedFiles += result.completedRequests;
			} while (result.status == Squall::ResolveStatus::ONGOING);

			if (result.status == Squall::ResolveStatus::ERROR) {
				spdlog::error("Could not complete remote download.");
			} else {
				spdlog::info("Downloaded {} files.", downloadedFiles);
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
					spdlog::error("Could not find root with name {}.", *rootName);
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
			spdlog::info("Realizing manifest '{}' into '{}'.", signatureInstance.str_view(), *directoryPath);

			Squall::iterator iterator(repository, *manifestResult.manifest);

			Realizer realizer(repository, *directoryPath, iterator, *realizeConfig);

			RealizeResult result{};
			do {
				result = realizer.poll();
			} while (result.status == Squall::RealizeStatus::INPROGRESS);

			if (result.status == Squall::RealizeStatus::INCOMPLETE) {
				spdlog::error("Could not complete realization.");
			} else {
				spdlog::info("Completed realizing files into '{}'.", *directoryPath);
			}
		});
	}
	CLI11_PARSE(app, argc, argv)
	return 0;
}