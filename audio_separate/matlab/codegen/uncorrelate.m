function Y = uncorrelate(a, b )
%#codegen
X = [a, b]';
Y = sqrtm(inv(cov(X')))*X;

end

