# Tests

This document briefly introduces test methodologies and the test environment used in WSNet project. Testing is an integral part of software development. Poor or non-existing testing methodologies lead to unstable products and unpredictable development times. It is very essential to have a testing plan in place to ensure that the product delivered is robust and stable.

## Test Methodology

Software testing methodologies are the different approaches and ways of ensuring that a software application in particular is fully tested. Standard development methodologies describe a set of general testing mechanisms which must be incorporated in the product development life-cycle. These mechanisms start from testing very small of code piece by piece to testing the whole application functionality in the end.

### Unit Tests

The Unit testing part of a testing methodology is the testing of individual software modules or components that make up an application or system. It covers very narrow and well defined scope, focusing on very small unit of functionality. They provide a simple way to check smallest units of code and prove that units can work perfectly in isolation. We isolate the code from any outside interaction or any dependency on any module. These tests are usually written by the developers of the component. Each component function is tested by a specific unit test fixture written in the same programming language as the module.

### Integration Tests

The Integration testing part of a testing methodology is the testing of the different modules/components that have been successfully unit tested when integrated together to perform specific tasks and activities. They provide a mechanism to test the interoperation of smaller units. They reveal out the issues with the connections between small units of code. Often when the units are wired together there are issues. Unit A might not be giving data properly to the Unit B and these issues come up in the integration tests.

### End-to-End Tests

After the integration tests are performed, higher level tests are used. The end-to-end, or system testing part of a testing methodology involves testing the entire system for errors and bugs. End-to-End tests check for the correctness of the output with respect to the input defined in the specification. Not much emphasis is given on the intermediate values but more focus is given on the final output delivered. This testing is listed under the black-box testing method, where the software is checked for user-expected working conditions as well as potential exception and edge conditions.


### Testing Pyramid

The right balance between all three test types, the best visual aid to use is the testing pyramid. Here is a simplified version of the testing pyramid:

<img src="img/testing_pyramid.png" width="300" hspace="500">

The majority of your tests are unit tests at the bottom of the pyramid. As you move up the pyramid, your tests gets larger, but at the same time the number of tests (the width of your pyramid) gets smaller.

As a good first guess, testing experts often suggests a 70/20/10 split: 70% unit tests, 20% integration tests, and 10% end-to-end tests. The exact mix will be different for each team, but in general, it should retain that pyramid shape. Try to avoid these anti-patterns:

 - Inverted pyramid. 
 - The team relies primarily on end-to-end tests, using few integration tests and even fewer unit tests. 
 - Hourglass. The team starts with a lot of unit tests, then uses end-to-end tests where integration tests should be used. The hourglass has many unit tests at the bottom and many end-to-end tests at the top, but few integration tests in the middle.
 
Just like a regular pyramid tends to be the most stable structure in real life, the testing pyramid also tends to be the most stable testing strategy.

A more complete discussion on the topic can be found [here][blog_google_testing].

## WSNet Tests Environment

The environment used in our tests is composed of several tools. The tests performed by WSNet are :

 - Build Tests (CMake & Make)
 - Memory check - Static Analysis (cppcheck)
 - Unit Tests 
 - Integration Tests
 - Memory Check - Dynamic Analysis (valgrind) using integration tests
 - End-to-End Tests

### CTest

The majority of WSNet tests are described using [CTest][ctest]. The tests can be run manually with the ctest command. As an example, the following command will run cppcheck tests:

```bash
ctest -R cppcheck
```
 
However, running them manually is counter productive. Thus, we use automated tools as Travis-ci.

### Travis-ci

WSNet project uses Travis-ci to test code that have been pushed to the github server. It passes the code committed through a series of tests, namely, build, cppcheck and integration tests.

The tests can ```fail``` or ```pass```. If it fails, Travis-ci will flag an error and the developer responsible for the commit must verify where the error occurred and fix it, before committing again.

[blog_google_testing]: https://testing.googleblog.com/2015/04/just-say-no-to-more-end-to-end-tests.html
[ctest]: https://cmake.org/Wiki/CMake/Testing_With_CTest
[travis-ci]: https://travis-ci.org/
[google_test]: https://github.com/google/googletest
