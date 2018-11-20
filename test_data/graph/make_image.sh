## 作成した画像をまとめる
convert +append density/density_mean_variance_0.png density/density_mean_variance_1.png density/density_mean_variance_6.png density/density_mean_variance_7.png density_1.png
convert +append density/density_mean_variance_10.png density/density_mean_variance_11.png density/density_mean_variance_2.png density/density_mean_variance_3.png density_2.png
convert +append density/density_mean_variance_12.png density/density_mean_variance_13.png density/density_mean_variance_4.png density/density_mean_variance_5.png density_3.png
convert +append density/density_mean_variance_8.png density/density_mean_variance_9.png density/density_mean_variance_14.png density/density_mean_variance_15.png density_4.png
convert -append density_1.png density_2.png density_3.png density_4.png density.png
rm density_*.png

convert +append real_density/density_mean_variance_0.png real_density/density_mean_variance_1.png real_density/density_mean_variance_6.png real_density/density_mean_variance_7.png density_1.png
convert +append real_density/density_mean_variance_10.png real_density/density_mean_variance_11.png real_density/density_mean_variance_2.png real_density/density_mean_variance_3.png density_2.png
convert +append real_density/density_mean_variance_12.png real_density/density_mean_variance_13.png real_density/density_mean_variance_4.png real_density/density_mean_variance_5.png density_3.png
convert +append real_density/density_mean_variance_8.png real_density/density_mean_variance_9.png real_density/density_mean_variance_14.png real_density/density_mean_variance_15.png density_4.png
convert -append density_1.png density_2.png density_3.png density_4.png real_density.png
rm density_*.png

convert -append mode/mode_probability_0.png mode/mode_probability_5.png mode/mode_probability_3.png mode/mode_probability_1.png mode_1.png
convert -append mode/mode_probability_6.png mode/mode_probability_4.png mode/mode_probability_2.png mode/mode_probability_7.png mode_2.png
convert +append mode_1.png mode_2.png mode.png
rm mode_*.png

convert +append real_density/density_mean_variance_0.png real_density/density_mean_variance_1.png real_density/density_mean_variance_6.png real_density/density_mean_variance_7.png real_density_1.png
convert +append real_density/density_mean_variance_10.png real_density/density_mean_variance_11.png real_density/density_mean_variance_2.png real_density/density_mean_variance_3.png real_density_2.png
convert +append real_density/density_mean_variance_12.png real_density/density_mean_variance_13.png real_density/density_mean_variance_4.png real_density/density_mean_variance_5.png real_density_3.png
convert +append real_density/density_mean_variance_8.png real_density/density_mean_variance_9.png real_density/density_mean_variance_14.png real_density/density_mean_variance_15.png real_density_4.png
convert -append real_density_1.png real_density_2.png real_density_3.png real_density_4.png real_density.png
rm real_density_*.png
