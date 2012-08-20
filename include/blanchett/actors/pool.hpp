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

#ifndef BLANCHETT_POOL_HPP_IS_INCLUDED
#define BLANCHETT_POOL_HPP_IS_INCLUDED

#include <blanchett/actors/thread.hpp>
#include <blanchett/actors/simple_reactor.hpp>
#include <blanchett/actors/quit.hpp>

// [mlr][todo] review header file usage.
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread.hpp>
#include <nyan/fail.hpp>

#include <sstream>

namespace blanchett
{

// [mlr][todo] should this be basic_pool?
template <
   class Message,
   class Service = simple_reactor< Message > >
class pool :
	public consumer< Message >
{
   private: typedef pool< Message, Service > this_type;
   public: typedef Message message_type;
   public: typedef Service service_type;
   private: typedef thread< message_type, service_type > thread_type;
   private: typedef std::vector< thread_type * > threads_type;
   public: typedef size_t size_type;

	// [mlr] i can't inherit from boost::thread_group because the author
	// decided to use a non-virtual destructor.
   private: threads_type my_threads;
	private: boost::thread_group my_group;

public:

   pool(size_type threadcnt_arg)
   {
      start(threadcnt_arg);
   }

	pool()
	{
	   start();
	}

	virtual ~pool()
	{
	   stop();
	}

	virtual void push(const message_type &msg_arg)
	{
	   // [mlr][todo] the affinity should probably be obtained through
	   // a traits class.
	   my_threads[msg_arg->affinity() % size()]->push(msg_arg);
	}

protected:

	void start(size_type threadcnt_arg)
	{
		// one cannot start an empty thread group.
		if (0 == threadcnt_arg)
			NYAN_FAIL_IFZERO(threadcnt_arg);
		else
		{
			// [mlr] i use a barrier to ensure that that all service loops start
			// at the same time. otherwise, there could be race conditions
			// during their initialization phase. i need to add 1 because
			// my thread is going to wait on this barrier too.
			// [mlr][todo] does Service need to support an
			// explicit initialization phase? i think perhaps it does.
			boost::barrier *ready = new boost::barrier(threadcnt_arg + 1);
			// i now start up each service loop on a separate thread.
			my_threads.reserve(threadcnt_arg);
			for (size_type i = 0; i < threadcnt_arg; ++i)
			{
			   std::ostringstream name;
			   name << "pool thread " << i;
	         thread_type * const t =
	               new thread_type(*ready, name.str());
	         my_threads.push_back(t);
				my_group.create_thread(boost::ref(*t));
			}
			// now, i wait for all of the threads to signal that
			// they're ready to run their main loop.
			std::cout << "main pool thread waiting for barrier."
			      << std::endl;
			ready->wait();
         std::cout << "main pool thread continuing."
               << std::endl;
		}
	}

	void start()
	{
		// [mlr][todo] i need to detect the optimal number of threads, given
		// the number of available CPUs.
		start(3);
	}

	size_type size() const
	{
      return my_group.size();
	}

	void stop()
	{
      std::cout << "main pool thread sending quit job."
            << std::endl;

      job::smart_ptr q(new quit());
      const typename threads_type::iterator i_end = my_threads.end();
      for (typename threads_type::iterator i = my_threads.begin(); i != i_end; ++i)
         (*i)->push(q);
      std::cout << "main pool thread waiting for children to terminate."
            << std::endl;
      my_group.join_all();
      static nyan::deletes< thread_type > f;
      std::for_each(my_threads.begin(), my_threads.end(), f);
	}

};// class pool

} //namespace blanchett

#endif // BLANCHETT_POOL_HPP_IS_INCLUDED

// $vim:23: vim:set sts=3 sw=3 et:,$
