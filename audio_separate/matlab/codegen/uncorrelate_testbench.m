[a, Fs1] = audioread('000000110mix1.wav');
[b, Fs2] = audioread('000000110mix2.wav');
Y = uncorrelate(a, b);

aa = a(1:256)'
bb = b(1:256)'
X = [aa ; bb]
cov(X')
