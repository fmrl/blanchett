// $legal:1626:
// 
// Copyright (c) 2011, Michael Lowell Roberts.  
// All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met: 
// 
//   - Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer. 
// 
//   - Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution.
//  
//   - Neither the name of the copyright holder nor the names of 
//   contributors may be used to endorse or promote products derived 
//   from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER 
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
// 
// ,$

#ifndef BLANCHETT_JOB_HPP_IS_INCLUDED
#define BLANCHETT_JOB_HPP_IS_INCLUDED

#include <boost/functional/hash.hpp>
#include <boost/random.hpp>
#include <boost/cstdint.hpp>
#include <nyan/ptr.hpp>

namespace blanchett
{

class job :
	public nyan::reference_counted_object
{
public:

	typedef boost::uint32_t affinity_type;
	typedef nyan::untrusted_smart_ptr< job > smart_ptr;

	private: affinity_type my_affinity;

public:

	job();
	job(affinity_type affinity_arg);
	virtual ~job();

	affinity_type affinity() const
	{
		return my_affinity;
	}

	virtual bool service() = 0;

	bool operator()()
	{
		return service();
	}

private:

	static affinity_type default_affinity();

	void initialize(affinity_type affinity_arg);

};

}//namespace blanchett

#endif //!defined(BLANCHETT_JOB_HPP_IS_INCLUDED)

// $vim:23: vim:set sts=3 sw=3 et:,$
