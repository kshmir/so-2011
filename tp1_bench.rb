if `uname -a`.match(/ubuntu\-vm/)
  Process.waitpid(fork { exec("make tp1") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method fifos --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method sockets --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method message_queues --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method shared_memory --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
else
  `cp -rf src/* ../so-2011/src`
  Dir.chdir("../so-2011")
  Process.waitpid(fork { exec("make tp1") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method fifos --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method sockets --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method message_queues --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  `./delete_ipcs.sh | pkill tp1_test_child | pkill tp1_test_server | pkill tp1_test | pkill tp1_level | pkill tp1_airline | rm tmp/*`
  Process.waitpid(fork { exec("./tp1 --method shared_memory --level files/mapa.txt --airline files/aerolinea.txt files/aerolinea.txt files/aerolinea.txt") })
  Dir.chdir("../so")
end

