%% init
[a, Fs1] = audioread('000000110mix1.wav');
[b, Fs2] = audioread('000000110mix2.wav');
N = min(length(a), length(b)); % need to be same length, so chop the longer one...
ab = [a, b]'; % 2abN row vector
% plot(a); hold on; plot(b);

%% sample data
ab = ab(:,1:256);  % arbitrary smaller sample for demo

%% center data
ab_mean = repmat(mean(ab,2),1,size(ab,2));  % tmp 2abN row vector
ab_centered = ab - ab_mean;  % subtrack dc bias

%% visualize, compute cov and inv(cov)
% cov is a measure of how the variables are dispersed around the mean 
    % (the diagonal elements) and how they co-vary with other variables 
    % (the off-diagonal) elements. The more the dispersion the farther apart 
    % they are from the mean and the more they co-vary (in absolute value) with 
    % the other variables the stronger is the tendency for them to 
    % 'move together' (in the same or opposite direction depending on the 
    % sign of the covariance).
scatter(a, b, '.'); hold on 
plot([mean(a), mean(a)], [-1, 1], 'Color', 'r')  % vert line for a_mean
plot([-1, 1], [mean(b), mean(b)], 'Color', 'r')  % horiz line for b_mean
ab_cov = cov(ab')  % need input column vector Nb

% we seek a linear transformation x of the data ab_norm such that 
% when P = x*ab_norm we now have cov(P) = I. It means that all the rows 
% of the transformed input data matrix are uncorrelated.

% Let Y * ab_cov = ab_norm..... find x
Y = sqrtm(inv(ab_cov)) * ab_centered  % not super sure about sqrtm
a_plus_b = repmat(sum(Y.*Y,1),size(Y,1),1);  % row a + b duplicated (2 x N)
[U,S,V] = svd((a_plus_b.*Y)*Y');
ab_unmix = U*ab; % U is unmixing matrix

%% Plot
subplot(2,2,1); plot(a); title('mixed audio - mic 1');
subplot(2,2,2); plot(b); title('mixed audio - mic 2');
subplot(2,2,3); plot(ab_unmix(1,:), 'g'); title('unmixed wave 1');
subplot(2,2,4); plot(ab_unmix(2,:),'r'); title('unmixed wave 2');

%% Write to disk
% audiowrite('unmixed1.wav', a(1,:), Fs1);
% audiowrite('unmixed2.wav', a(2,:), Fs1);
