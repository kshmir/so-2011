
extra_args = "--level files/default/mapa.txt --airline files/default/aerolinea0.txt files/default/aerolinea1.txt files/default/aerolinea2.txt files/default/aerolinea3.txt files/default/aerolinea4.txt files/default/aerolinea5.txt files/default/aerolinea6.txt files/default/aerolinea7.txt files/default/aerolinea8.txt files/default/aerolinea9.txt"

if `uname -a`.match(/ubuntu\-vm/) and Dir.pwd == "so"
  `cp -rf src/* ../so-2011/src`
#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm ./so-2011/tmp/*`
  Dir.chdir("../so-2011")
  if ARGV[0] != "nocomp"
      Process.waitpid(fork { exec("make tp1") })
  end
#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method fifos #{extra_args}") })
 
  #{}`./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
 
  Process.waitpid(fork { exec("./tp1 --method sockets #{extra_args}") })
 
 # `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method message_queues #{extra_args}") })

#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method shared_memory #{extra_args}") })
  Dir.chdir("../so")
else
  if ARGV[0] != "nocomp"
      Process.waitpid(fork { exec("make tp1") })
  end
#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method fifos #{extra_args}") })

#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method sockets #{extra_args}") })

#  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method message_queues #{extra_args}") })

#    `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
    Process.waitpid(fork { exec("./tp1 --method shared_memory #{extra_args}") })

end

