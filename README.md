# StringHandler
C++ If string starts with specific keyword call lambda function.

Checks incoming string from function "search" if matches any filter executes the lambda function a "executer thread" which is created upon class init.

To add keyword to check, call "add_filter" first param is filter name, second param is keyword to check for.

To assign a lambda function you must call "on" first param filter name, second param lambda function.
