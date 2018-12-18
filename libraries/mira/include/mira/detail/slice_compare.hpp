#pragma once

#include <rocksdb/slice.h>

#include <fc/io/raw.hpp>

#include <assert.h>

namespace mira { namespace multi_index { namespace detail {

template< typename Key, typename CompareType >
struct abstract_slice_comparator : ::rocksdb::Comparator, CompareType
{
   abstract_slice_comparator() {}

   virtual const char* Name() const override final
   {
      static const std::string name = boost::core::demangle(typeid(this).name());
      return name.c_str();
   }

   virtual void FindShortestSeparator( std::string* start, const ::rocksdb::Slice& limit ) const override final
   {
      /// Nothing to do.
   }

   virtual void FindShortSuccessor( std::string* key ) const override final
   {
      /// Nothing to do.
   }
};

template< typename Key, typename CompareType >
struct slice_comparator final : abstract_slice_comparator< Key, CompareType >
{
   slice_comparator() : abstract_slice_comparator< Key, CompareType >()
   {}

   virtual int Compare( const ::rocksdb::Slice& x, const ::rocksdb::Slice& y ) const override
   {
      //assert( x.size() == y.size() );

      int r = (*this)(
         fc::raw::unpack_from_char_array< Key >( x.data(), x.size() ),
         fc::raw::unpack_from_char_array< Key >( y.data(), y.size() )
      );

      if( r ) return -1;

      if( ( x.size() == y.size() ) && memcmp( x.data(), y.data(), x.size() ) == 0 ) return 0;

      return 1;
   }

   virtual bool Equal( const ::rocksdb::Slice& x, const ::rocksdb::Slice& y ) const override
   {
      assert( x.size() == y.size() );
      return memcmp( x.data(), y.data(), x.size() ) == 0;
   }
};

// TODO: Primitive Types can implement custom comparators that understand the independent format

} } } // mira::multi_index::detail