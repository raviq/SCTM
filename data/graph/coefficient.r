file_path <- "./../result/time/sctm_result_necessary_time_"
output_path <-  "./../result/coefficient/sctm_result_coefficient_"
img_path <- "./coefficient/sctm_result_coefficient_"
link_num <- 136

for (i in 0:link_num){
  file_name <- paste(file_path, ".csv", sep=as.character(i))
  data <- read.csv(file_name, head=T);
  names(data) <- c("Time", "Mean", "Plus", "Minus", "SD")
  coefficient <- data.frame(Time=data$Time, CV=(data$SD / data$Mean))

  # 出力データの保存
  output_name <-  paste(output_path, ".csv", sep=as.character(i))
  write.csv(coefficient, output_name)

  # グラフの描画と保存
  img_name <- paste(img_path, ".png", sep = as.character(i))
  png(file = img_name)

  time <- coefficient$Time
  cv <- coefficient$FF
  x_max <- max(time)
  x_min <- min(time)
  y_max <- max(cv)
  y_min <- min(cv)

  plot(time, cv, xlab="Time", ylab="Coefficient", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(y_min,y_max))

  dev.off()
}

