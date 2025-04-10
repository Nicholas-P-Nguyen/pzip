## COS417 Spring 2025: pzip

In this assignment, you will build a parallelized text compression tool: **pzip**. 
The type of compression used here is a simple form of compression called
*run-length encoding* (*RLE*). RLE is quite simple: when you encounter **n**
characters of the same type in a row, the compression tool (**pzip**) will
turn that into the number **n** followed by a single instance of the character.

Thus, if we had a file with the following contents:
```
aaaaaaaaaabbbb
```
the tool would turn it into:
```
10a4b
```
(Hint:) This applies for **all** characters, including spaces and line breakers!

However, the exact format of the compressed file is quite important; here,
you will write out a **4-byte** integer in binary format followed by the single
character in ASCII. Thus, a compressed file will consist of some number of
5-byte entries, each of which is comprised of a 4-byte integer (the run
length) and the single character. 

To write out an integer in binary format (not ASCII), you should use
**fwrite()**. Read the man page for more details. For **pzip**, all
output should be written to standard output (the **stdout** file stream,
which, as with **stdin**, is already open when the program starts running). 

**Implementation**

We've provided some starter framework for you in `pzip.c`; as always, you are free to define/utilize more classes, functions, structs etc. however you like; everything that's provided only serves as a guidance! 

The framework contains a main function and a pzip function; read through the comments
and annotations to gain a better idea of the general flow of the program. In a nutshell:
- Handle incorrect usages of pzip. Correct invocation should pass a file and an integer > 0 via the command line to the program, e.g. `./pzip book.txt 4`; in this case meaning "please compress the file called `book.txt` using 4 threads." If the format is incorrect, the program should exit with return code 1 and print `pzip: file num_threads`; followed by a newline. 
- Access the file that was passed in as an argument, and `mmap` that file's contents into memory. This allows you to process and compress the file in a faster and parallelizable way.
- Given the number of threads specified in the argument, spawn that many threads and assign equal portions of the file to each thread for them to compress. Be sure to handle any remainders as well!
- The starter code provides a `pzip()` function that's intended to be where your compression logic would go. If you choose to use it, `pzip()` would be the function that each spawned thread would call to compress the workload they were given. 
- Develop a way to aggregate the compressed results computed by each thread. The ultimate goal is to write the compressed results to stdout. Note: the order in which you write the compressed results matter, so `pthread_join` may be helpful!
- After writing all results, be sure to cleanup after yourself, i.e. unmap, close up files, and free up any space you might've allocated.

**Testing**

You can test your code by simply running `./run_test.sh`, which supports the same flags as before. The public tests are contained in the `tests` directory. 

**More Details & Hints**

* Be sure to consider consecutive sequences of newline characters (`\n`) as well! As with any other character, if the program sees only one `\n`, then it's corresponding compressed output should just be "1\n" (in 5 bytes).
* The starter code also provides a `MAX_COMPRESSED_SIZE` constant that defines the maximum amount of 5-byte entries every thread can expect to compute for any tests (including hidden) ran on your pzip program. You may use that to your advantage if necessary.
* There will be hidden tests for this assignment; the provided tests are only there to serve as a guidance for you to do you own testing! Be sure to conduct testing yourself to more thoroughly evaluate the robustness of your solution (hint; such as calling your program on a large corpus of text with a high number of threads (~8-10))
* Since displaying 5-byte entries might be difficult for some text editors, tools like `hexdump` might be very helpful when debugging. You can use the -C flag to view your outputs byte-by-byte, but beware of the endianess of the outputs.

**Submission**

You'll be uploading your `pzip.c` file (NOT the executable) to TigerFile under the pzip assignment - all public test cases will be run whenever you click on 'check submitted files'.
