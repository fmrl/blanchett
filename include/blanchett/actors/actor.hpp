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

#ifndef BLANCHETT_ACTOR_HPP_IS_INCLUDED
#define BLANCHETT_ACTOR_HPP_IS_INCLUDED

#include <blanchett/actors/consumer.hpp>
#include <blanchett/actors/job.hpp>
#include <blanchett/actors/pool.hpp>
#include <blanchett/actors/reactor.hpp>
#include <blanchett/actors/stl_queue_adaptor.hpp>

#include <boost/bind.hpp>
#include <nyan/ptr.hpp>

#include <cassert>

namespace blanchett
{

template <
	class Message,
	class Queue = stl_queue_adaptor< Message >
>
class actor :
   public job,
   public reactor< Message >
{
private:

   public: typedef Message message_type;
   public: typedef Queue queue_type;
   private: typedef actor< Message, Queue > this_type;
   private: typedef reactor< Message > reactor_type;
   public: typedef nyan::untrusted_smart_ptr< this_type > smart_ptr;
   // [mlr][todo] both reactor and pool are consumers, so it seems i should
   // reintroduce the consumer abstraction.
   public: typedef consumer< job::smart_ptr > ready_queue_type;

   private: ready_queue_type * const my_readyq;
   private: queue_type my_queue;

public:

	actor(ready_queue_type &readyq_arg) :
	   my_readyq(&readyq_arg)
	{}

	actor(ready_queue_type &readyq_arg, affinity_type affinity_arg) :
		job(affinity_arg),
		my_readyq(readyq_arg)
	{}

	virtual void push(const message_type &msg_arg)
	{
	   if (my_queue.push(msg_arg))
	      schedule();
	}

	virtual bool service()
	{
		// i need to guarantee that i'll be in one thread at a time but i want
		// to do it without locking while i invoke consume(). i can obstruct the
		// queue from emptying (and therefore allowing for another thread to
		// schedule me before i'm finished) by refusing to pop() anything i'm
		// ready to be rescheduled.
		message_type msg;
		// TODO: i can optimize my_queue.front() by returning a reference
		// but it would be a special-case optimization because i can't normally
		// assume that that would be thread-safe.
		my_queue.front(msg);
		// once i've consumed 'i', i can let another thread have a go. 
		// i do this by pop() the item off the stack. if it doesn't leave the queue empty, 
		// my scheduler will schedule me to consume something else off of the queue.
		process(msg);
      // placing myself on the ready queue must occur atomically along with
      // popping the last item off of the queue. if it isn't, a producer
      // thread could place me on the ready queue first, if it beats me to
      // it with a push() operation. this would put me on the ready queue
      // twice, violating one of the constraints i need in order to operate.
      my_queue.pop(msg,
            boost::bind(std::mem_fun(&this_type::schedule), this));
      return true;
	}

protected:

	virtual void process(const message_type &msg_arg) = 0;

	// [mlr][todo] should there be a templated multicast object that i can
	// << into?
	template < class U >
	void multicast(const U &to_arg, const Message &what_arg) const
	{
		// [mlr][todo] type check; U must be a sequence of actors.
		std::for_each(to_arg.begin(), to_arg.end(),
				U::value_type::message_type::operator<<);
	}

private:

	virtual void finalize() throw()
   {
      job::finalize();
   }

	void schedule()
	{
	   my_readyq->push(this);
	}


};// class actor

}//namespace banchett

#endif // BLANCHETT_ACTOR_HPP_IS_INCLUDED

// $vim:23: vim:set sts=3 sw=3 et:,$
