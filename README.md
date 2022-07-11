# VirtualMAN
Hybrid virtual machine I've written for a school project.

# Purpose of this project

This is a project I've made for my 1st year apprenticeship as a software engineer in Switzerland. </br>
I was able to choose a free project, so I've chosen a virtual machine project and this is the result. </br>
Do note that not everything works perfectly fine right now, I'm not really planning to update the virtual machine. </br>
People who reviewed my project have stated that the project far exceeds the expectations for a 17 year old programmer and I'm happy to hear about this. </br>
I've decided to release the project to the public, so anyone can take a look at my approach of programming a virtual machine in C++.</br>

It was a great journey and I hope this project inspires some people to into virtual machine programming. The project itself is relatively small</br>
and the unreleased project documentation (which I still do keep private and might release it at a later date) is more complex.</br>

Do with the project what you want as long as it doesn't violate the MIT license.

# How it works

The virtual machine itself is a register based virtual machine, many people state that a stack based one is easier to program, for me, it is the opposite.</br>
VirtualMAN hosts a dyncall instance that works stack based and is capable of calling native functions at runtime. In the repository, you'll find a test file</br>
that contains nothing but VirtualMAN opcodes that pushes strings to the dyncall stack and calls the C puts function.</br>

# How to execute and disassemble

Please note that the test binaries were not created with a personal developed compiler/assembler. I had not enough time left to program one.</br>
## vman -e test.bin - Execute binary
## vman -d test.bin - Disassemble binary
