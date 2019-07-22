// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011 Erik Ogenvik.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_OBJECTS_FWD_H
#define ATLAS_OBJECTS_OBJECTS_FWD_H

namespace Atlas { namespace Objects { 

template <class T> class SmartPtr;

class ObjectsDecoder;

class ObjectsEncoder;

class Dispatcher;

class RootData;
typedef SmartPtr<RootData> Root;

namespace Entity {
    class RootEntityData;
    typedef SmartPtr<RootEntityData> RootEntity;
}

namespace Entity {
    class AdminEntityData;
    typedef SmartPtr<AdminEntityData> AdminEntity;
}

namespace Entity {
    class AccountData;
    typedef SmartPtr<AccountData> Account;
}

namespace Entity {
    class PlayerData;
    typedef SmartPtr<PlayerData> Player;
}

namespace Entity {
    class AdminData;
    typedef SmartPtr<AdminData> Admin;
}

namespace Entity {
    class GameData;
    typedef SmartPtr<GameData> Game;
}

namespace Entity {
    class GameEntityData;
    typedef SmartPtr<GameEntityData> GameEntity;
}

namespace Operation {
    class RootOperationData;
    typedef SmartPtr<RootOperationData> RootOperation;
}

namespace Operation {
    class ActionData;
    typedef SmartPtr<ActionData> Action;
}

namespace Operation {
    class CreateData;
    typedef SmartPtr<CreateData> Create;
}

namespace Operation {
    class CombineData;
    typedef SmartPtr<CombineData> Combine;
}

namespace Operation {
    class DivideData;
    typedef SmartPtr<DivideData> Divide;
}

namespace Operation {
    class CommunicateData;
    typedef SmartPtr<CommunicateData> Communicate;
}

namespace Operation {
    class TalkData;
    typedef SmartPtr<TalkData> Talk;
}

namespace Operation {
    class DeleteData;
    typedef SmartPtr<DeleteData> Delete;
}

namespace Operation {
    class SetData;
    typedef SmartPtr<SetData> Set;
}

namespace Operation {
    class AffectData;
    typedef SmartPtr<AffectData> Affect;
}

namespace Operation {
    class MoveData;
    typedef SmartPtr<MoveData> Move;
}

namespace Operation {
    class WieldData;
    typedef SmartPtr<WieldData> Wield;
}

namespace Operation {
    class GetData;
    typedef SmartPtr<GetData> Get;
}

namespace Operation {
    class PerceiveData;
    typedef SmartPtr<PerceiveData> Perceive;
}

namespace Operation {
    class LookData;
    typedef SmartPtr<LookData> Look;
}

namespace Operation {
    class ListenData;
    typedef SmartPtr<ListenData> Listen;
}

namespace Operation {
    class SniffData;
    typedef SmartPtr<SniffData> Sniff;
}

namespace Operation {
    class TouchData;
    typedef SmartPtr<TouchData> Touch;
}

namespace Operation {
    class LoginData;
    typedef SmartPtr<LoginData> Login;
}

namespace Operation {
    class LogoutData;
    typedef SmartPtr<LogoutData> Logout;
}

namespace Operation {
    class ImaginaryData;
    typedef SmartPtr<ImaginaryData> Imaginary;
}

namespace Operation {
    class UseData;
    typedef SmartPtr<UseData> Use;
}

namespace Operation {
    class InfoData;
    typedef SmartPtr<InfoData> Info;
}

namespace Operation {
    class PerceptionData;
    typedef SmartPtr<PerceptionData> Perception;
}

namespace Operation {
    class SightData;
    typedef SmartPtr<SightData> Sight;
}

namespace Operation {
    class AppearanceData;
    typedef SmartPtr<AppearanceData> Appearance;
}

namespace Operation {
    class DisappearanceData;
    typedef SmartPtr<DisappearanceData> Disappearance;
}

namespace Operation {
    class SoundData;
    typedef SmartPtr<SoundData> Sound;
}

namespace Operation {
    class SmellData;
    typedef SmartPtr<SmellData> Smell;
}

namespace Operation {
    class FeelData;
    typedef SmartPtr<FeelData> Feel;
}

namespace Operation {
    class UnseenData;
    typedef SmartPtr<UnseenData> Unseen;
}

namespace Operation {
    class ErrorData;
    typedef SmartPtr<ErrorData> Error;
}

namespace Operation {
    class ChangeData;
    typedef SmartPtr<ChangeData> Change;
}

namespace Entity {
    class AnonymousData;
    typedef SmartPtr<AnonymousData> Anonymous;
}

namespace Operation {
    class GenericData;
    typedef SmartPtr<GenericData> Generic;
}

} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_OBJECTS_FWD_H
