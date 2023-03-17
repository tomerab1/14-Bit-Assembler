# 14 bit Assembler

The final assignment of the 'System programming' course in the Open University.

This project is based on the **_two-pass assembler_** model.  
**Note:** the computer model for this project and the given assembly language are **imaginary**.

## Getting Started

The project was targeted to comply the `C90 standard` and meant to be run on Linux env.

### Usage

Use makefile to compile the project (and clean trailing object files) as follows:

```
>   make
>   make clean
```

After preparing assembly files **with an `.as` extension**, open _terminal_ and pass file names as arguments (without the file extensions) as following:

As for the files x.as, y.as, hello.as we will run:

```
>   assembler x y hello
```

The assembler will generate output files with the same filenames and the following extensions:

- `.ob` - Object file
- `.ent` - Entries file
- `.ext` - Externals file
- `.as` - Pre-assembled file (after macro expansion).

An example of input and output files can be found under the 'tests' folder.

## Hardware

- CPU
- RAM (including a stack), with the size of 256 _words_.
- A _word_'s size in memory is **14 bits**.
- Uses signed _2's complement_ arithmetic for integers (with no support for real numbers).

### Registers

The CPU has 9 registers: 8 general registers (named r1,r2,...,r8).

## Instruction Structure

Every software instruction is encoded into a few words in memory (max is 4 _words_).
The first word is of the following structure:

| 13&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;12 | 11&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;10 | 9&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;8&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;7&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;6 | 5&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 3&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | 1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0&nbsp;&nbsp;&nbsp; |
| :--------------------------------: | :--------------------------------: | :--------------------------------------------------------------------------------------------: | :------------------------------------------------------------------------: | :------------------------------------------------------------: | :------------------------------------------------: |
|              Param 1               |              Param 2               |                                             Opcode                                             |                               Source operand                               |                          Dest operand                          |                       E.R.A                        |

`Thanks for Chat gpt for generating this table`

Encoding of each instruction word is done using the following format:

```
0: .
1: /
```

each word can be represented as 2 bytes, where we only use _8 bits_ from the first byte, and _6 bits_ from the second.

## Commands

The commands allowed in bits 6-9 are:

| Opcode (decimal) | Command Name |
| ---------------- | ------------ |
| 0                | mov          |
| 1                | cmp          |
| 2                | add          |
| 3                | sub          |
| 4                | not          |
| 5                | clr          |
| 6                | lea          |
| 7                | inc          |
| 8                | dec          |
| 9                | jmp          |
| 10               | bne          |
| 11               | red          |
| 12               | prn          |
| 13               | jsr          |
| 14               | rts          |
| 15               | stop         |

## Directives

A **_directive_** line of the following structure:

1. An **optional** preceding _label_. e.g. `PLACE1: `.
2. A _directive_: `.data`, `.string`, `.entry` or `.extern`.
3. Operands according to the type of the _directive_.

   ### `.data`

   This directive allocates memory in the data image for storing received integers later in memory (also increases the _data counter_ and updates the _symbol table_).
   The parameters of `.data` are valid integers (separated by a comma).  
   e.g. `LABEL1: .data      +7, -56, 4, 9`.

   ### `.string`

   This direcive receives a string as an operand and stores it in the data image. It stores all characters by their order in the string, encoded ny their _ASCII_ values.
   e.g. `STRING1: .string "abcdef"` is a valid directive.

   ### `.entry`

   This directive outputs a received name of a _label_ to the _symbol table_, so that later it will be recognized by other assembly files (and they would be able to use it).
   e.g.

   ```
   ; file1.as
   .entry HELLO
   HELLO: add #1, r1
   ```

   ### `.extern`

   This directive receives a name of a _label_ as a parameter and declares the _label_ as being external (defined in another file) and that the current file shall use it.  
    This way, the directive `.extern HELLO` in `file2.as` will match the `.entry` directive in the previous example.
