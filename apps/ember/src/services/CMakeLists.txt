find_package(Vorbis REQUIRED)

add_library(ember-services STATIC
        config/ConfigListener.cpp
        config/ConfigListenerContainer.cpp
        config/ConfigService.cpp
        config/ConfigConsoleCommands.cpp
        input/Input.cpp
        input/InputCommandMapper.cpp
        input/EmberIcon.h
        input/IInputAdapter.h
        metaserver/MetaserverService.cpp
        scripting/ScriptingService.cpp
        server/AccountAvailableState.cpp
        server/AvatarTransferInfo.cpp
        server/ConnectedState.cpp
        server/ConnectingState.cpp
        server/Connection.cpp
        server/EnteredWorldState.cpp
        server/LocalServerAdminCreator.cpp
        server/LoggedInState.cpp
        server/NonConnectedState.cpp
        server/OOGChat.cpp
        server/ServerService.cpp
        server/ServerServiceConnectionListener.cpp
        server/TransferInfoStringSerializer.cpp
        server/IConnectionListener.h
        server/IState.h
        server/ServerServiceSignals.h
        server/StateBase.h
        server/AssetsSyncNeededState.cpp
        serversettings/ServerSettings.cpp
        serversettings/ServerSettingsCredentials.cpp
        sound/SoundGeneral.cpp
        sound/SoundSample.cpp
        sound/SoundService.cpp
        sound/SoundSource.cpp
        EmberServices.cpp)

target_link_libraries(ember-services PUBLIC
        ember-framework
        ember-external-mojoal
        varconf
        Vorbis::vorbisfile
)


file(GLOB BINDINGS_FILES bindings/lua/*.cpp)

add_library(ember-BindingsServices STATIC
        ${BINDINGS_FILES}
)
target_link_libraries(ember-BindingsServices PUBLIC
        ember-services
        ember-external-sol2
)

target_precompile_headers(ember-BindingsServices
        PUBLIC
        <sol2/sol.hpp>
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsServices PRIVATE "/bigobj")
endif ()
