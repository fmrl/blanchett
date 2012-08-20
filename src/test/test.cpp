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

#include <blanchett/actors.hpp>

class actor :
   public blanchett::actor< int >
{
   private: typedef blanchett::actor< int > super_type;

public:

   actor(ready_queue_type &readyq_arg) :
      super_type(readyq_arg)
   {}

protected:

   virtual void process(const int &msg_arg)
   {
      std::cout << "actor " << this << ": processed " << msg_arg << "."
            << std::endl;
   }

};

int main()
{

   blanchett::pool< blanchett::job::smart_ptr > pool;
   std::vector< actor::smart_ptr > actors;

   for (int i = 0; i < 5; ++i)
      actors.push_back(new actor(pool));

   for (int i = 0; i < 100; ++i)
      actors[i % actors.size()]->push(i);

   std::cout << "main thread sleeping." << std::endl;

   sleep(10);

   std::cout << "main thread done." << std::endl;

   return 0;
}

// $vim:23: vim:set sts=3 sw=3 et:,$
