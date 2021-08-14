close all; clear all;
h = fopen('heightfield.raw'); assert(h > 0); d = fread(h, 'uint16'); fclose(h);
imagesc(reshape(d, [1 1] * sqrt(numel(d))).');