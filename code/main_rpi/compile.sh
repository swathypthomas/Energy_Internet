gcc initialize_DB.c peer_daemon.c -o /home/pi/main_rpi/executables/initialize_DB -L/home/pi/redis-stable/deps/hiredis -lhiredis
gcc listen.c pub_sub.c -o /home/pi/main_rpi/executables/listen -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc ack.c pub_sub.c -o /home/pi/main_rpi/executables/ack -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc soc_low.c pub_sub.c -o /home/pi/main_rpi/executables/soc_low -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc soc_high.c pub_sub.c -o /home/pi/main_rpi/executables/soc_high -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc srtstp_transfer.c pub_sub.c -o /home/pi/main_rpi/executables/srtstp_transfer -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc srt_process.c pub_sub.c -o /home/pi/main_rpi/executables/srt_process -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc stp_process.c pub_sub.c -o /home/pi/main_rpi/executables/stp_process -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc sense_disconnect.c pub_sub.c -o /home/pi/main_rpi/executables/sense_disconnect -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc fwd_req.c pub_sub.c -o /home/pi/main_rpi/executables/fwd_req -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc display_board.c -o /home/pi/main_rpi/executables/display_board -L/home/pi/redis-stable/deps/hiredis -lhiredis -levent
gcc peer_ping.c peer_daemon.c -o /home/pi/main_rpi/executables/peer_ping -L/home/pi/redis-stable/deps/hiredis -lhiredis
