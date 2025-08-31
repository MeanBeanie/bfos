#!/bin/bash

if [[ "$1" == "" ]]; then
	echo "need a commit message"
	exit
fi

git add *
git rm -f --cached limine # TODO figure out a better way to handle this
git commit -m "$1"
git push
