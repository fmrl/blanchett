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

#ifndef BLANCHETT_STL_QUEUE_ADAPTOR_HPP_IS_INCLUDED
#define BLANCHETT_STL_QUEUE_ADAPTOR_HPP_IS_INCLUDED

#include <nyan/functions/nop.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/bind.hpp>

#include <cassert>
#include <functional>
#include <queue>

namespace blanchett
{

template <
	class Value,
	class Queue = std::queue< Value >
>
class stl_queue_adaptor
{
public:

	typedef Value value_type;
	typedef typename Queue::size_type size_type;

private:

	typedef Queue stl_queue_type;
	typedef stl_queue_adaptor< Value, Queue > this_type;
	typedef boost::mutex mutex_type;
	typedef boost::unique_lock< mutex_type > lock_type;

	mutable mutex_type my_mutex;
	mutable boost::condition_variable my_condition;
	stl_queue_type my_stl_queue;
	
public:

	stl_queue_adaptor()
	{}

	virtual ~stl_queue_adaptor()
	{}

	bool empty() const
	{
		lock_type mine(my_mutex);
		return my_stl_queue.empty();
	}

	bool ready() const
	{
      lock_type mine(my_mutex);
      return unguarded_ready();
	}

	bool push(value_type what_arg)
	{
		bool was_empty = false;
		{
			lock_type mine(my_mutex);
			was_empty = my_stl_queue.empty();
			my_stl_queue.push(what_arg);
		}
		// [mlr] awaken someone wait()ing to know it's no longer empty.
		// [mlr][todo] i'm not entirely sure why i can't signal only when
		// was_empty -> true. the debugger suggests that it deadlocks
		// only when the quit() command is issued, so there's something
		// going on there that i don't quite understand. signaling
		// every time i put something into the queue appears to work,
		// so i'm going with that for now even though it's less
		// efficient than the ideal.
		my_condition.notify_one();
		return was_empty;
	}

	void front(value_type &result_arg) const
	{
		return wait_front(result_arg);
	}

   bool front_if_ready(value_type &result_arg) const
   {
      lock_type mine(my_mutex);
      if (my_stl_queue.empty())
         return false;
      else
      {
         result_arg = my_stl_queue.front();
         return true;
      }
   }

	template < class F >
	void pop(value_type &result_arg, F moar_arg)
	{
		wait_pop(result_arg, moar_arg);
	}

	void pop(value_type &result_arg)
	{
		static const nyan::nop nop;
		(void)pop(result_arg, nop);
	}

	void pop()
	{
		static const nyan::nop nop;
		pop(nop);
	}

	template < class F >
	void pop(F moar_arg)
	{
		value_type discarded;
		pop(discarded, moar_arg);
	}

	bool pop_if_ready(value_type &result_arg)
	{
		static const nyan::nop nop;
		return pop_if_ready(result_arg, nop);
	}

	template < class F >
	bool pop_if_ready(value_type &result_arg, F moar_arg)
	{
		lock_type mine(my_mutex);
		if (my_stl_queue.empty())
			return false;
		else
		{
         // [mlr] if that was the last item in the queue, invoke the
		   // caller-provided handler.
			if (!unguarded_pop(result_arg))
				moar_arg();
			return true;
		}
	}

private:

	template < class F >
	void wait_pop(value_type &result_arg, F moar_arg)
	{
		lock_type mine(my_mutex);
		// [mlr] if there's nothing to pick off of the queue, wait for
		// something.
		my_condition.wait(mine,
		      boost::bind(std::mem_fun(&this_type::unguarded_ready), this));
		// [mlr] i'm guaranteed to have something in my queue now. if that
		// was not the last item in the queue, invoke the caller-provided
		// handler.
		if (!unguarded_pop(result_arg))
			moar_arg();
	}

	void wait_front(value_type &result_arg) const
	{
		lock_type mine(my_mutex);
		// [mlr] if there's nothing to pick off of the queue, wait for
		// something.
		my_condition.wait(mine,
		      boost::bind(std::mem_fun(&this_type::unguarded_ready), this));
		// [mlr] i'm guaranteed to have something in my queue now. if that
		// was not the last item in the queue, invoke the caller-provided
		// handler.
		result_arg = my_stl_queue.front();
	}

	bool unguarded_pop(value_type &result_arg)
	{
		// [mlr] warning: the mutex is expected to be held by the caller.
		result_arg = my_stl_queue.front();
		my_stl_queue.pop();
		// [mlr] inform the caller whether i left the queue empty.
		return my_stl_queue.empty();
	}

	bool unguarded_ready() const
	{
	   return !my_stl_queue.empty();
	}

};

} //namespace blanchett

#endif /* BLANCHETT_STL_QUEUE_ADAPTOR_HPP_IS_INCLUDED */

// $vim:23: vim:set sts=3 sw=3 et:,$
