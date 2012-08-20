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

#ifndef BLANCHETT_THREAD_HPP_IS_INCLUDED
#define BLANCHETT_THREAD_HPP_IS_INCLUDED

#include <blanchett/actors/actor.hpp>

#include <boost/thread/barrier.hpp>

#include <string>

namespace blanchett
{

template < class Message, class Service >
class thread :
   public consumer< Message >
{
   public: typedef Message message_type;
   public: typedef Service service_type;

	private: boost::barrier *my_start_barrier;
	private: service_type my_service;
	private: const std::string my_name;

public:

	thread() :
		my_start_barrier(NULL)
	{}

	thread(boost::barrier &start_barrier_arg, const std::string &name_arg) :
		my_start_barrier(&start_barrier_arg),
		my_name(name_arg)
	{}

	virtual void push(const message_type &msg_arg)
	{
	   my_service.push(msg_arg);
	}

	void operator()()
	{
		// [mlr] first, i signal that i'm ready and wait until all of my
	   // siblings are ready.

	   std::cout << "thread \"" << my_name << "\" waiting for barrier."
	         << std::endl;

		my_start_barrier->wait();
		// [mlr] i should not need the barrier again.
		my_start_barrier = NULL;

      std::cout << "thread \"" << my_name << "\" beginning service loop."
            << std::endl;

		// [mlr] now, i run the thread's main loop until it's time to exit.
		while (my_service.service())
		   continue;

      std::cout << "thread \"" << my_name << "\" finished."
            << std::endl;
	}

};

} // namespace blanchett

#endif // BLANCHETT_THREAD_HPP_IS_INCLUDED

// $vim:23: vim:set sts=3 sw=3 et:,$
