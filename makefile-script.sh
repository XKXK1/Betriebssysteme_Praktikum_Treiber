#!/bin/bash

bash remove.sh

sudo make clean

sudo make RET_VAL_NUMBER=-5 RET_VAL_TIME=-5

bash setup.sh

