#ifndef ERIS_PERL_ATLAS_CONVERSION_H
#define ERIS_PERL_ATLAS_CONVERSION_H

#include <Atlas/Message/Object.h>

#include "refcount.h"

SV* AtlasToSV(const Atlas::Message::Element &obj)
{
  switch(obj.getType()) {
    case Atlas::Message::Element::TYPE_NONE:
      return NEWSV(0, 0);
    case Atlas::Message::Element::TYPE_INT:
      return newSViv(obj.asInt());
    case Atlas::Message::Element::TYPE_FLOAT:
      return newSVnv(obj.asFloat());
    case Atlas::Message::Element::TYPE_STRING:
      {
        const std::string &str = obj.asString();
        return newSVpv(str.c_str(), str.size());
      }
    case Atlas::Message::Element::TYPE_MAP:
      {
        HV *hv = newHV();
        const Atlas::Message::Element::MapType &map = obj.asMap();
        Atlas::Message::Element::MapType::const_iterator I;
        for(I = map.begin(); I != map.end(); ++I)
             hv_store(hv, I->first.c_str(), I->first.size(), AtlasToSV(I->second), 0);
        return newRV_noinc((SV*) hv);
      }
    case Atlas::Message::Element::TYPE_LIST:
      {
        AV *av = newAV();
        const Atlas::Message::Element::ListType &list = obj.asList();
        av_extend(av, list.size() - 1);
        for(I32 i; i < list.size(); ++i)
          av_store(av, i, AtlasToSV(list[i]));
        return newRV_noinc((SV*) av);
      }
    default:
      assert(false);
  }
}

namespace SigCPerl {
  inline SV* GetSV(const Atlas::Message::Element &obj) {return AtlasToSV(obj);}
}

void SVToAtlas(SV *sv, Atlas::Message::Element& obj)
{
  svtype type = (svtype) SvTYPE(sv);
  if(type == SVt_RV)
    type = (svtype) SvTYPE((SV*) SvRV(sv));

  switch(type) {
    case SVt_IV:
      obj = SvIV(sv);
      break;
    case SVt_NV:
      obj = SvNV(sv);
      break;
    case SVt_PV:
      {
        STRLEN len;
        const char *str = SvPV(sv, len);
        obj = std::string(str, len);
      }
      break;
    case SVt_PVAV:
      {
        AV* av = (AV*) SvRV(sv);
        I32 len = av_len(av);
        assert(len >= -1);
        obj = Atlas::Message::Element::ListType(len + 1);
        if(len == -1)
          break;
        assert(obj.isList());
        Atlas::Message::Element::ListType& list = obj.asList();
        for(I32 i = 0; i <= len; ++i) {
          SV **val = av_fetch(av, i, 0);
          assert(val);
          SVToAtlas(*val, list[i]);
        }
      }
      break;
    case SVt_PVHV:
      {
        HV* hv = (HV*) SvRV(sv);
        obj = Atlas::Message::Element::MapType();
	assert(obj.isMap());
	Atlas::Message::Element::MapType& map = obj.asMap();
        I32 havekeys = hv_iterinit(hv);
        while(havekeys) {
          char *key;
          SV *val = hv_iternextsv(hv, &key, &havekeys);
          SVToAtlas(val, map[key]);
        }
      }
      break;
    case SVt_NULL: // undef -> TYPE_NONE (?)
      obj = Atlas::Message::Element();
      break;
    default:
      throw Atlas::Message::WrongTypeException();
  }
}

#endif // ERIS_PERL_ATLAS_CONVERSION_H
