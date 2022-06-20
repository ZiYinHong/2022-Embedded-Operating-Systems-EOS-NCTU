#!/bin/bash

SESSION="lab7"
KEY="1234"
GUESS="3"

make clean
make

tmux has-session -t $SESSION 2>/dev/null

if [[ $? -eq 0 ]]
then
	tmux kill-session -t $SESSION
fi

tmux new-session -d -s $SESSION

tmux split-window -h

tmux send-keys -t 0 "./game $KEY $GUESS" C-m
tmux send-keys -t 1 "./guess $KEY 100"

tmux -2 attach-session -t $SESSION
