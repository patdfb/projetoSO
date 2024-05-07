#!/bin/bash

echo bin/client execute 10 -u "hello 10"
echo bin/client execute 5 -u "void 5"
echo bin/client execute 40 -u "void 4"
echo bin/client execute 25 -u "hello 6"
echo bin/client execute 10 -p cat "hello 10" | grep "hello" | wc -l

