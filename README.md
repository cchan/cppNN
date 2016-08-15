# cppNN

Originally created for Lexington High School Science Fair 2015.
A neural network library written from scratch, and associated attempts to train it.

I wrote a C++ deep neural network implementation from scratch using tanh layers. I 
used both the easier-to-understand but slower genetic algorithm and the harder-to-
understand but faster backpropagation algorithm; the library is capable of both types 
of learning. In this process I learned a huge amount about the capabilities of C++ as 
a language, and of many subtle aspects of data structures, algorithms, and compilers, 
all in an effort to make the neural network as efficient as possible.

I trained this library first on the zero output vector, which it learned successfully, 
especially after I added simulated annealing (which, I like to brag, I came up with 
independently ;) ) and meta-annealing (I made that second one up; it seems effective 
but I'm not certain if it is).

I then trained it on a more complex situation, in which small "robots" roamed about a 
field, with a "predator" species and a "prey" species. The network did seem to learn 
some interesting behaviors to either dodge or catch the other species, and there even 
seemed to be a predator-prey cycle. However, there was far too much noise in the data 
to reliably say anything about the results. In conclusion, it's a reasonably well-
written library, but I don't know how to train it properly.

More recently, I also used this project to learn how to write Makefiles, which are 
actually really cool. (Previously, this project had been using Visual C++ 2015.)
