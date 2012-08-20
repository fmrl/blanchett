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

#ifndef BLANCHETT_SIMPLE_REACTOR_HPP_IS_INCLUDED
#define BLANCHETT_SIMPLE_REACTOR_HPP_IS_INCLUDED

#include <blanchett/actors/stl_queue_adaptor.hpp>
#include <blanchett/actors/reactor.hpp>

namespace blanchett
{

template <
   class Message,
   class Queue = stl_queue_adaptor< Message >
>
class simple_reactor :
   public reactor< Message >
{
   private: typedef simple_reactor< Message > this_type;
   public: typedef Message message_type;
   public: typedef Queue queue_type;

   private: queue_type my_queue;

public:

	simple_reactor()
	{}

	virtual ~simple_reactor()
	{}

	virtual void push(const message_type &msg_arg)
	{
	   (void)my_queue.push(msg_arg);
	}

	virtual bool service()
	{
	   message_type msg;
	   (void)my_queue.pop(msg);
	   return msg->service();
	}

};

}//namespace blanchett

#endif //!defined(BLANCHETT_SIMPLE_REACTOR_HPP_IS_INCLUDED)

// $vim:23: vim:set sts=3 sw=3 et:,$
