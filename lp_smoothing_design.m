clear;
close all;

fs      = 44100;				    % Sampling frequency of 44100 Hz
fpass   = 5;                        % Passband edge frequency of 10750 Hz
Wp      = fpass / (fs/2);			% Calculate normalized passband frequency
fstop   = 10;                        % Stopband edge frequency of 16540 Hz
Ws      = fstop / (fs/2);			% Calculate normalized stopband frequency
Rp_lin  = 0.01;						% Max. passband ripple of 0.01
Rp      = 20 * log10(1+Rp_lin);		% Calculate passband ripple in dB
Rs      = 40;					    % Min. stopband attenuation of 40dB


%                   Design the elliptic lowpass filter

% Calculate the order of the elliptic lowpass filter

[N, W] = ellipord(Wp, Ws, Rp, Rs); 

% Calculate the coefficients of the elliptic lowpass filter via Bilinear Transformation

[b_ellip_lp, a_ellip_lp] = ellip(N, Rp, Rs, W, 'low');

% Convert the filter coefficients to second-order sections 

[sos_ellip_lp, g_ellip_lp] = tf2sos(b_ellip_lp, a_ellip_lp); 
%
% Distribute the gain equally over the sections
[N_sos, ~] = size(sos_ellip_lp);

sos_ellip_lp(:,1:3) = sos_ellip_lp(:,1:3) * (g_ellip_lp^(1/N_sos));

%
% Normalize the coefficients 

%sos_ellip_lp = sos_ellip_lp / max(max(abs(sos_ellip_lp)));

% Write the second-order sections to a header file

write_float_sos_coeff('C:\Users\gunja\workspace_v7\CJ1 - Effect pedal\effects\smoothing_filter.h', 'num_smooth', 'den_smooth', 'N_sos_smooth', sos_ellip_lp);

% Plot the 16-bit coefficients of the Elliptic low pass

% Quantize the coefficients for plotting

sos_ellip_lp_16bit = round(sos_ellip_lp * 2^(16-1));

figure(1)
%freqz(sos_ellip_lp_16bit);
freq = (0.1:9999)/20000;
hz = freqz(sos_ellip_lp_16bit, 2 * pi * freq);
semilogx(freq * fs, db(hz));
grid on;
title('16-bit Elliptic low-pass filter');

figure(2)
%freqz(sos_ellip_lp);
%freq = (-0.1:9999)/20000;
freq = 0.5 * logspace(-5, -0.01, 10000);
hz = freqz(sos_ellip_lp, 2 * pi * freq);
semilogx(freq * fs, db(hz));
grid on;
%title('16-bit Elliptic low-pass filter');
%title('Elliptic low-pass filter');
%%					Design the Chebyshev low-pass filter

% Calculate the order of the chebyshev lowpass filter

[N_iir_lp, W] = cheb2ord(Wp, Ws, Rp, Rs); 

% Calculate the coefficients of the chebyshev lowpass filter

[b_cheby_lp, a_cheby_lp] = cheby2(N_iir_lp, Rp, W, 'low');


% Convert the filter coefficients to Second Order Section

[sos_cheby_lp, g_cheby_lp] = tf2sos(b_cheby_lp, a_cheby_lp); 

% Distribute the gain equally over the sections

[N_sos, ~] = size(sos_cheby_lp);
sos_cheby_lp(:,1:3) = sos_cheby_lp(:,1:3) * (g_cheby_lp^(1/N_sos));


% Normalize the coefficients

%sos_cheby_lp = sos_cheby_lp / max(max(abs(sos_cheby_lp)));

% Write the second-order sections to a header file

write_float_sos_coeff('C:\Users\gunja\workspace_v7\CJ1 - Effect pedal\effects\smoothing_filter.h', 'num_smooth', 'den_smooth', 'N_sos_smooth', sos_cheby_lp);

% Plot the 16-bit coefficients of the Chebyshev low pass

% Quantize the coefficients for plotting

sos_cheby_lp_16bit = round(sos_cheby_lp * 2^(16-1));

figure(1)
freqz(sos_cheby_lp);
title('Chebyshev low-pass filter');

figure(2)
freqz(sos_cheby_lp_16bit);
title('16-bit Chebyshev low-pass filter');

%%					Design the Butterworth low-pass filter

% Calculate the order of the chebyshev lowpass filter

[N_iir_lp, W] = buttord(Wp, Ws, Rp, Rs); 

% Calculate the coefficients of the chebyshev lowpass filter

[z, p, k] = butter(N_iir_lp, W, 'low');


% Convert the filter coefficients to Second Order Section

[sos_butter_lp, g_butter_lp] = zp2sos(z, p, k); 

% Distribute the gain equally over the sections

[N_sos, ~] = size(sos_butter_lp);
sos_butter_lp(:,1:3) = sos_butter_lp(:,1:3) * (g_butter_lp^(1/N_sos));


% Normalize the coefficients

%sos_butter_lp = sos_butter_lp / max(max(abs(sos_butter_lp)));

% Write the second-order sections to a header file

write_float_sos_coeff('C:\Users\gunja\workspace_v7\CJ1 - Effect pedal\effects\smoothing_filter.h', 'num_smooth', 'den_smooth', 'N_sos_smooth', sos_butter_lp);

% Plot the 16-bit coefficients of the Chebyshev low pass

% Quantize the coefficients for plotting

sos_butter_lp_16bit = round(sos_butter_lp * 2^(16-1));

figure(1)
freqz(sos_butter_lp);
title('Butterworth low-pass filter');
%%
figure(2)
freqz(sos_butter_lp_16bit);
title('16-bit Butterworth low-pass filter');

%% Design an FIR filter


% Input parameters
fs = 44100;             % Sampling frequency: 44,1 kHz
fcuts = [200 400];    % Passband edge: 4500 Hz, Stopband edge: 6000 Hz
pbr = 0.01;             % Max. passband ripple: 0.01
sba_db = 40;            % Min. stopband attenuation: 40 dB 
sba = 10^(sba_db/20);   % Calculate non-decibel value
sbr = 1/sba;            % Stopband ripple
devs = [pbr sbr];       % Create deviations vector
mags = [1 0];           % Amplitude range from 1 to 0

% Output parameters
[N_coe_LP, fo, mo, w] = firpmord(fcuts, mags, devs, fs);
N_coe_LP = N_coe_LP+1;
b_FIR_coe_LP = firpm(N_coe_LP, fo, mo, w);
b_FIR_coe_LP = firpm(20,[0 0.03 0.1 1],[1 1 0 0]);
% Print values to header file using write_coeff.m
fid = fopen('FIR_LP.h', 'w'); % open the header file
write_coeff(fid, 'LP_FIR_coe', b_FIR_coe_LP, length(b_FIR_coe_LP), 16);
fclose(fid); % close the header file

%% Plot and compare the frequenc responses of the 16-bit and 8-bit filters

% Visualize the lowpass and highpass filters
% a = 2^(bits-1) to correct the magnitude for visualization

lowpass_graphs = fvtool(b_FIR_coe_LP);
legend(lowpass_graphs, 'FIR Lowpass');

