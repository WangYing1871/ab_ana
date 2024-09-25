// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIhomedIwangyingdIdesktopdIab_softdIzhangrydIanalysiscode_version6_2dIab_anadIroot_cint_out
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "ROOT/RConfig.hxx"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "data_strcut_cint.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *waveform_pack_Dictionary();
   static void waveform_pack_TClassManip(TClass*);
   static void *new_waveform_pack(void *p = nullptr);
   static void *newArray_waveform_pack(Long_t size, void *p);
   static void delete_waveform_pack(void *p);
   static void deleteArray_waveform_pack(void *p);
   static void destruct_waveform_pack(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::waveform_pack*)
   {
      ::waveform_pack *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::waveform_pack));
      static ::ROOT::TGenericClassInfo 
         instance("waveform_pack", "data_strcut_cint.h", 9,
                  typeid(::waveform_pack), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &waveform_pack_Dictionary, isa_proxy, 0,
                  sizeof(::waveform_pack) );
      instance.SetNew(&new_waveform_pack);
      instance.SetNewArray(&newArray_waveform_pack);
      instance.SetDelete(&delete_waveform_pack);
      instance.SetDeleteArray(&deleteArray_waveform_pack);
      instance.SetDestructor(&destruct_waveform_pack);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::waveform_pack*)
   {
      return GenerateInitInstanceLocal(static_cast<::waveform_pack*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::waveform_pack*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *waveform_pack_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::waveform_pack*>(nullptr))->GetClass();
      waveform_pack_TClassManip(theClass);
   return theClass;
   }

   static void waveform_pack_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *tq_pack_Dictionary();
   static void tq_pack_TClassManip(TClass*);
   static void *new_tq_pack(void *p = nullptr);
   static void *newArray_tq_pack(Long_t size, void *p);
   static void delete_tq_pack(void *p);
   static void deleteArray_tq_pack(void *p);
   static void destruct_tq_pack(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::tq_pack*)
   {
      ::tq_pack *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::tq_pack));
      static ::ROOT::TGenericClassInfo 
         instance("tq_pack", "data_strcut_cint.h", 23,
                  typeid(::tq_pack), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &tq_pack_Dictionary, isa_proxy, 0,
                  sizeof(::tq_pack) );
      instance.SetNew(&new_tq_pack);
      instance.SetNewArray(&newArray_tq_pack);
      instance.SetDelete(&delete_tq_pack);
      instance.SetDeleteArray(&deleteArray_tq_pack);
      instance.SetDestructor(&destruct_tq_pack);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::tq_pack*)
   {
      return GenerateInitInstanceLocal(static_cast<::tq_pack*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::tq_pack*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *tq_pack_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::tq_pack*>(nullptr))->GetClass();
      tq_pack_TClassManip(theClass);
   return theClass;
   }

   static void tq_pack_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *entry_Dictionary();
   static void entry_TClassManip(TClass*);
   static void *new_entry(void *p = nullptr);
   static void *newArray_entry(Long_t size, void *p);
   static void delete_entry(void *p);
   static void deleteArray_entry(void *p);
   static void destruct_entry(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::entry*)
   {
      ::entry *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::entry));
      static ::ROOT::TGenericClassInfo 
         instance("entry", "data_strcut_cint.h", 37,
                  typeid(::entry), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &entry_Dictionary, isa_proxy, 0,
                  sizeof(::entry) );
      instance.SetNew(&new_entry);
      instance.SetNewArray(&newArray_entry);
      instance.SetDelete(&delete_entry);
      instance.SetDeleteArray(&deleteArray_entry);
      instance.SetDestructor(&destruct_entry);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::entry*)
   {
      return GenerateInitInstanceLocal(static_cast<::entry*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::entry*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *entry_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::entry*>(nullptr))->GetClass();
      entry_TClassManip(theClass);
   return theClass;
   }

   static void entry_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *entry_new_Dictionary();
   static void entry_new_TClassManip(TClass*);
   static void *new_entry_new(void *p = nullptr);
   static void *newArray_entry_new(Long_t size, void *p);
   static void delete_entry_new(void *p);
   static void deleteArray_entry_new(void *p);
   static void destruct_entry_new(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::entry_new*)
   {
      ::entry_new *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::entry_new));
      static ::ROOT::TGenericClassInfo 
         instance("entry_new", "data_strcut_cint.h", 52,
                  typeid(::entry_new), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &entry_new_Dictionary, isa_proxy, 0,
                  sizeof(::entry_new) );
      instance.SetNew(&new_entry_new);
      instance.SetNewArray(&newArray_entry_new);
      instance.SetDelete(&delete_entry_new);
      instance.SetDeleteArray(&deleteArray_entry_new);
      instance.SetDestructor(&destruct_entry_new);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::entry_new*)
   {
      return GenerateInitInstanceLocal(static_cast<::entry_new*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::entry_new*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *entry_new_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::entry_new*>(nullptr))->GetClass();
      entry_new_TClassManip(theClass);
   return theClass;
   }

   static void entry_new_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *collection_tree_Dictionary();
   static void collection_tree_TClassManip(TClass*);
   static void *new_collection_tree(void *p = nullptr);
   static void *newArray_collection_tree(Long_t size, void *p);
   static void delete_collection_tree(void *p);
   static void deleteArray_collection_tree(void *p);
   static void destruct_collection_tree(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::collection_tree*)
   {
      ::collection_tree *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::collection_tree));
      static ::ROOT::TGenericClassInfo 
         instance("collection_tree", "data_strcut_cint.h", 61,
                  typeid(::collection_tree), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &collection_tree_Dictionary, isa_proxy, 0,
                  sizeof(::collection_tree) );
      instance.SetNew(&new_collection_tree);
      instance.SetNewArray(&newArray_collection_tree);
      instance.SetDelete(&delete_collection_tree);
      instance.SetDeleteArray(&deleteArray_collection_tree);
      instance.SetDestructor(&destruct_collection_tree);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::collection_tree*)
   {
      return GenerateInitInstanceLocal(static_cast<::collection_tree*>(nullptr));
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal(static_cast<const ::collection_tree*>(nullptr)); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *collection_tree_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal(static_cast<const ::collection_tree*>(nullptr))->GetClass();
      collection_tree_TClassManip(theClass);
   return theClass;
   }

   static void collection_tree_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_waveform_pack(void *p) {
      return  p ? new(p) ::waveform_pack : new ::waveform_pack;
   }
   static void *newArray_waveform_pack(Long_t nElements, void *p) {
      return p ? new(p) ::waveform_pack[nElements] : new ::waveform_pack[nElements];
   }
   // Wrapper around operator delete
   static void delete_waveform_pack(void *p) {
      delete (static_cast<::waveform_pack*>(p));
   }
   static void deleteArray_waveform_pack(void *p) {
      delete [] (static_cast<::waveform_pack*>(p));
   }
   static void destruct_waveform_pack(void *p) {
      typedef ::waveform_pack current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::waveform_pack

namespace ROOT {
   // Wrappers around operator new
   static void *new_tq_pack(void *p) {
      return  p ? new(p) ::tq_pack : new ::tq_pack;
   }
   static void *newArray_tq_pack(Long_t nElements, void *p) {
      return p ? new(p) ::tq_pack[nElements] : new ::tq_pack[nElements];
   }
   // Wrapper around operator delete
   static void delete_tq_pack(void *p) {
      delete (static_cast<::tq_pack*>(p));
   }
   static void deleteArray_tq_pack(void *p) {
      delete [] (static_cast<::tq_pack*>(p));
   }
   static void destruct_tq_pack(void *p) {
      typedef ::tq_pack current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::tq_pack

namespace ROOT {
   // Wrappers around operator new
   static void *new_entry(void *p) {
      return  p ? new(p) ::entry : new ::entry;
   }
   static void *newArray_entry(Long_t nElements, void *p) {
      return p ? new(p) ::entry[nElements] : new ::entry[nElements];
   }
   // Wrapper around operator delete
   static void delete_entry(void *p) {
      delete (static_cast<::entry*>(p));
   }
   static void deleteArray_entry(void *p) {
      delete [] (static_cast<::entry*>(p));
   }
   static void destruct_entry(void *p) {
      typedef ::entry current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::entry

namespace ROOT {
   // Wrappers around operator new
   static void *new_entry_new(void *p) {
      return  p ? new(p) ::entry_new : new ::entry_new;
   }
   static void *newArray_entry_new(Long_t nElements, void *p) {
      return p ? new(p) ::entry_new[nElements] : new ::entry_new[nElements];
   }
   // Wrapper around operator delete
   static void delete_entry_new(void *p) {
      delete (static_cast<::entry_new*>(p));
   }
   static void deleteArray_entry_new(void *p) {
      delete [] (static_cast<::entry_new*>(p));
   }
   static void destruct_entry_new(void *p) {
      typedef ::entry_new current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::entry_new

namespace ROOT {
   // Wrappers around operator new
   static void *new_collection_tree(void *p) {
      return  p ? new(p) ::collection_tree : new ::collection_tree;
   }
   static void *newArray_collection_tree(Long_t nElements, void *p) {
      return p ? new(p) ::collection_tree[nElements] : new ::collection_tree[nElements];
   }
   // Wrapper around operator delete
   static void delete_collection_tree(void *p) {
      delete (static_cast<::collection_tree*>(p));
   }
   static void deleteArray_collection_tree(void *p) {
      delete [] (static_cast<::collection_tree*>(p));
   }
   static void destruct_collection_tree(void *p) {
      typedef ::collection_tree current_t;
      (static_cast<current_t*>(p))->~current_t();
   }
} // end of namespace ROOT for class ::collection_tree

namespace {
  void TriggerDictionaryInitialization_root_cint_out_Impl() {
    static const char* headers[] = {
"data_strcut_cint.h",
nullptr
    };
    static const char* includePaths[] = {
"/home/wangying/desktop/ab_soft/zhangry/analysiscode_version6_2/ab_ana",
"/home/wangying/software/root-6.30.04-build/include/",
"/home/wangying/Desktop/ab_soft/zhangry/analysiscode_version6_2/ab_ana/build/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "root_cint_out dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  waveform_pack;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  tq_pack;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  entry;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  entry_new;
struct __attribute__((annotate("$clingAutoload$data_strcut_cint.h")))  collection_tree;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "root_cint_out dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "data_strcut_cint.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"collection_tree", payloadCode, "@",
"entry", payloadCode, "@",
"entry_new", payloadCode, "@",
"tq_pack", payloadCode, "@",
"waveform_pack", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("root_cint_out",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_root_cint_out_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_root_cint_out_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_root_cint_out() {
  TriggerDictionaryInitialization_root_cint_out_Impl();
}
