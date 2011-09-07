c = 0;
  while c < 10 do
    Process.waitpid(fork { exec("./tp1 --method #{ARGV[0]} --level files/default/mapa.txt --airline files/default/aerolinea1.txt files/default/aerolinea2.txt files/default/aerolinea3.txt files/default/aerolinea4.txt files/default/aerolinea5.txt files/default/aerolinea6.txt files/default/aerolinea7.txt files/default/aerolinea8.txt files/default/aerolinea9.txt files/default/aerolinea0.txt ") })
    c += 1
  end
