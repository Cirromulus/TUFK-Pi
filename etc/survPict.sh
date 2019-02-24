fswebcam --png 0 /home/pi/surveil/$(date '+%Y-%m-%d-%H-%M-%S').png -r 640x480
cd /home/pi/surveil/
ls -t | sed -e '1,100d' | xargs -r -d '\n' rm
