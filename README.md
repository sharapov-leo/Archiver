# Manual

```
SYNOPSIS
  <sentence> ::= [arguments]* file_name [arguments]*
  
  <arguments> ::= -c | -d | -k | -r | -1 | -9

DESCRIPTION
    The archiver reduces the size of named files using BWT + MFT + RLE. Whenever possible, each file is replaced by one with the extension .sharp. If a filename is "-", the standard input is compressed to the standard output.
    
    Compressed files can be restored to their original form using -d.
    -c
        Write output on standard output; keep original files unchanged. If there are several input files, the output consists of a sequence of independently compressed members. To obtain better compression, concatenate all input files before compressing them.
    -d
        Decompress.
    -k
        Keep  (don't  delete)  input files during compression or decompression.
    -r
        Travel the directory structure recursively. If any of the file names specified on the command line are directories, gzip will descend into the directory and compress all the files it finds there (or decompress them in the case of gunzip ).
    -1
        The maximum compressible block size is 102400 bytes (100 KB).
    -9
        The maximum compressible block size is 921600 bytes (900 KB) by default.
```
