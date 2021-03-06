//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2018, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  List of MPEG PSI/SI descriptors
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsDescriptor.h"

namespace ts {
    //!
    //! List of MPEG PSI/SI descriptors.
    //!
    class TSDUCKDLL DescriptorList
    {
    public:
        //!
        //! Default constructor.
        //!
        DescriptorList() :
            _list()
        {
        }

        //!
        //! Copy constructor.
        //! The descriptors objects are shared between the two lists.
        //! @param [in] dl Another instance to copy.
        //!
        DescriptorList(const DescriptorList& dl) :
            _list(dl._list)
        {
        }

        //!
        //! Get the number of descriptors in the list.
        //! @return The number of descriptors in the list.
        //!
        size_t count() const
        {
            return _list.size();
        }

        //!
        //! Comparison operator.
        //! @param [in] other Another instance to compare.
        //! @return True if the two descriptor lists are identical.
        //!
        bool operator==(const DescriptorList& other) const;

        //!
        //! Comparison operator.
        //! @param [in] other Another instance to compare.
        //! @return True if the two descriptor lists are different.
        //!
        bool operator!=(const DescriptorList& other) const
        {
            return !(*this == other);
        }

        //!
        //! Get a reference to the descriptor at a specified index.
        //! @param [in] index Index in the list. Valid index are 0 to count()-1.
        //! @return A reference to the descriptor at @a index.
        //!
        const DescriptorPtr& operator[](size_t index) const
        {
            assert(index < _list.size());
            return _list[index].desc;
        }

        //!
        //! Return the "private data specifier" associated to a descriptor in the list.
        //! @param [in] index Index of a descriptor in the list. Valid index are 0 to count()-1.
        //! @return The "private data specifier" associated to a descriptor at @a index.
        //!
        PDS privateDataSpecifier(size_t index) const
        {
            assert(index < _list.size());
            return _list[index].pds;
        }

        //!
        //! Add one descriptor at end of list
        //! @param [in] desc The binary descriptor to add.
        //!
        void add(const DescriptorPtr& desc);

        //!
        //! Add one descriptor at end of list
        //! @param [in] desc The descriptor to add.
        //!
        void add(const AbstractDescriptor& desc);

        //!
        //! Add another list of descriptors at end of list.
        //! The descriptors objects are shared between the two lists.
        //! @param [in] dl The descriptor list to add.
        //!
        void add(const DescriptorList& dl)
        {
            _list.insert(_list.end(), dl._list.begin(), dl._list.end());
        }

        //!
        //! Add descriptors from a memory area at end of list
        //! @param [in] addr Address of descriptors in memory.
        //! @param [in] size Size in bytes of descriptors in memory.
        //!
        void add(const void* addr, size_t size);

        //!
        //! Add one descriptor from a memory area at end of list.
        //! The size is extracted from the descriptor header.
        //! @param [in] addr Address of the descriptor in memory.
        //!
        void add(const void* addr)
        {
            const uint8_t* data(reinterpret_cast<const uint8_t*>(addr));
            add(data, size_t(data[1]) + 2);
        }

        //!
        //! Add a private_data_specifier descriptor if necessary at end of list.
        //! If the current private data specifier at end of list is not @a pds,
        //! a private_data_specifier descriptor is added. If @a pds is already
        //! the current private data specifier, the list is unchanged.
        //! @param [in] pds A private data specifier.
        //!
        void addPrivateDataSpecifier(PDS pds);

        //!
        //! Remove the descriptor at the specified index in the list.
        //! A private_data_specifier descriptor can be removed only if
        //! it is not necessary (no private descriptor ahead).
        //! @param [in] index Index of the descriptor to remove.
        //! @return True on success, false on error (index out of range
        //! or required private_data_specifier descriptor).
        //!
        bool removeByIndex(size_t index);

        //!
        //! Remove all descriptors with the specified tag.
        //! A private_data_specifier descriptor can be removed only if
        //! it is not necessary (no private descriptor ahead).
        //! @param [in] tag Tag of descriptors to remove.
        //! @param [in] pds Private data specifier.
        //! If @a pds is non-zero and @a tag is >= 0x80, remove only
        //! descriptors with the corresponding private data specifier.
        //! @return The number of removed descriptors.
        //!
        size_t removeByTag(DID tag, PDS pds = 0);

        //!
        //! Remove all private descriptors without preceding private_data_specifier_descriptor.
        //! @return The number of removed descriptors.
        //!
        size_t removeInvalidPrivateDescriptors();

        //!
        //! Clear the content of the descriptor list.
        //!
        void clear()
        {
            _list.clear();
        }

        //!
        //! Search a descriptor with the specified tag.
        //! @param [in] tag Tag of descriptor to search.
        //! @param [in] start_index Start searching at this index.
        //! @param [in] pds Private data specifier.
        //! If @a pds is non-zero and @a tag is >= 0x80, return only
        //! a descriptor with the corresponding private data specifier.
        //! @return The index of the descriptor in the list or count() if no such descriptor is found.
        //!
        size_t search(DID tag, size_t start_index = 0, PDS pds = 0) const;

        //!
        //! Search a language descriptor for the specified language.
        //! @param [in] language The 3-character language name to search.
        //! @param [in] start_index Start searching at this index.
        //! @return The index of the descriptor in the list or count() if no such descriptor is found.
        //!
        size_t searchLanguage(const UString& language, size_t start_index = 0) const;

        //!
        //! Search any kind of subtitle descriptor.
        //! @param [in] language The language name to search.
        //! If @a language is non-empty, look only for a subtitle
        //! descriptor matching the specified language. In this case, if some
        //! kind of subtitle descriptor exists in the list but none matches the
        //! language, return count()+1.
        //! @param [in] start_index Start searching at this index.
        //! @return The index of the descriptor in the list or count() if no such descriptor is found.
        //!
        size_t searchSubtitle(const UString& language = UString(), size_t start_index = 0) const;

        //!
        //! Search a descriptor with the specified tag.
        //! @tparam DESC A subclass of AbstractDescriptor.
        //! @param [in] tag Tag of descriptor to search.
        //! @param [out] desc When a descriptor with the specified tag is found,
        //! it is deserialized into @a desc. Always check desc.isValid() on return
        //! to check if the deserialization was successful.
        //! @param [in] start_index Start searching at this index.
        //! @param [in] pds Private data specifier.
        //! If @a pds is non-zero and @a tag is >= 0x80, return only
        //! a descriptor with the corresponding private data specifier.
        //! @return The index of the descriptor in the list or count() if no such descriptor is found.
        //!
        template <class DESC>
        size_t search(DID tag, DESC& desc, size_t start_index = 0, PDS pds = 0) const;

        //!
        //! Total number of bytes that is required to serialize the list of descriptors.
        //! @return The total number of bytes that is required to serialize the list of descriptors.
        //!
        size_t binarySize() const;

        //!
        //! Serialize the content of the descriptor list.
        //! @param [in,out] addr Address of the memory area where the descriptors
        //! are serialized. Upon return, @a addr is updated to contain the next
        //! address in memory, after the last serialized byte.
        //! @param [in,out] size Size in bytes of the memory area where the descriptors
        //! are serialized. Upon return, @a size is updated to the remaining size
        //! of the buffer. Descriptors are written one by one until either the end
        //! of the list or until one descriptor does not fit.
        //! @param [in] start Start searializing at this index.
        //! @return The index of the first descriptor that could not be serialized
        //! (or count() if all descriptors were serialized). In the first case,
        //! the returned index can be used as @a start parameter to serialized the
        //! rest of the list (in another section for instance).
        //!
        size_t serialize(uint8_t*& addr, size_t& size, size_t start = 0) const;

        //!
        //! Same as serialize(), but prepend a 2-byte length field before the descriptor list.
        //! The 2-byte length field has 4 reserved bits (all '1') and 12 bits for the length
        //! of the descriptor list.
        //! @param [in,out] addr Address of the memory area where the descriptors
        //! are serialized. Upon return, @a addr is updated to contain the next
        //! address in memory, after the last serialized byte.
        //! @param [in,out] size Size in bytes of the memory area where the descriptors
        //! are serialized. Upon return, @a size is updated to the remaining size
        //! of the buffer. Descriptors are written one by one until either the end
        //! of the list or until one descriptor does not fit.
        //! @param [in] start Start searializing at this index.
        //! @return The index of the first descriptor that could not be serialized
        //! (or count() if all descriptors were serialized). In the first case,
        //! the returned index can be used as @a start parameter to serialized the
        //! rest of the list (in another section for instance).
        //!
        size_t lengthSerialize(uint8_t*& addr, size_t& size, size_t start = 0) const;

        //!
        //! This method converts a descriptor list to XML.
        //! @param [in,out] parent The parent node for the XML descriptors.
        //! @param [in] charset If not zero, default character set to use.
        //! @return True on success, false on error.
        //!
        bool toXML(xml::Element* parent, const DVBCharset* charset = 0) const;

        //!
        //! This method decodes an XML list of descriptors.
        //! @param [out] others Returned list of non-descriptor XML elements.
        //! All these elements are not null and their names are in @a allowedOthers.
        //! @param [in] parent The XML element containing all descriptors.
        //! @param [in] allowedOthers A list of allowed element names inside @a parent which are not descriptors.
        //! @param [in] charset If not zero, character set to use without explicit table code.
        //! @return True on success, false on error.
        //!
        bool fromXML(xml::ElementVector& others, const xml::Element* parent, const UStringList& allowedOthers, const DVBCharset* charset = 0);

        //!
        //! This method decodes an XML list of descriptors.
        //! @param [out] others Returned list of non-descriptor XML elements.
        //! All these elements are not null and their names are in @a allowedOthers.
        //! @param [in] parent The XML element containing all descriptors.
        //! @param [in] allowedOthers A comma-separated list of allowed element names inside @a parent which are not descriptors.
        //! @param [in] charset If not zero, character set to use without explicit table code.
        //! @return True on success, false on error.
        //!
        bool fromXML(xml::ElementVector& others, const xml::Element* parent, const UString& allowedOthers, const DVBCharset* charset = 0);

        //!
        //! This method decodes an XML list of descriptors.
        //! @param [in] parent The XML element containing all descriptors.
        //! All children must be valid descriptors.
        //! @return True on success, false on error.
        //!
        bool fromXML(const xml::Element* parent);

    private:
        // Each entry contains a descriptor and its corresponding private data specifier.
        struct Element
        {
            // Public members:
            DescriptorPtr desc;
            PDS pds;

            // Constructor:
            Element(const DescriptorPtr& desc_ = 0, PDS pds_ = 0) : desc(desc_), pds(pds_) {}
        };
        typedef std::vector <Element> ElementVector;

        // Private members
        ElementVector _list;

        // Prepare removal of a private_data_specifier descriptor.
        // Return true if can be removed, false if it cannot (private descriptors ahead).
        // When it can be removed, the current PDS of all subsequent descriptors is updated.
        bool prepareRemovePDS(const ElementVector::iterator&);
    };
}

#include "tsDescriptorListTemplate.h"
