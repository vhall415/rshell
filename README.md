#My command shell

This rshell has basic functionality.
It can print a command prompt and read in a command line.
Command lines have the format of
``executable [argumentList] [connector cmd]``
The connector will be either || or && or ;
Anything typed after # will be considered a comment
The shell can be exited with the command `exit`

The redirection operations (<, >, >>, <<<) can be used to redirect inputs and output.

#Bugs to fix

Errors with piping
