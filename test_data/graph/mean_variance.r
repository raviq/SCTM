## シミュレーションの結果を描画（密度の平均と分散）
# ファイル読み込み
file_path <-  "./../result/density/sctm_result_density_"
img_path <- "./density/density_mean_variance_"
img_path2 <- "./real_density/density_mean_variance_"
cell_num <- 15

for (i in 0:cell_num){
  name <- paste(file_path, ".csv", sep = as.character(i));
  data <- read.csv(name, head=T);
  names(data) <- c("Time", "Mean", "Plus", "Minus", "SD", "Real")

  # グラフの描画と保存
  file_name <- paste(img_path, ".png", sep = as.character(i))
  png(file = file_name)

  time <- data$Time
  mean <- data$Mean
  plus <- data$Plus
  minus <- data$Minus
  x_max <- max(time)
  x_min <- min(time)
  y_max <- max(c(mean,plus,minus))
  y_min <- min(c(mean,plus,minus))
  # y_max <- 400
  # y_min <- 0

  plot(time, mean, xlab="Time", ylab="Density", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(y_min,y_max)); par(new=T)
  plot(time, plus, xlab="", ylab="", type = "l", col="blue", xlim=c(x_min,x_max), ylim=c(y_min,y_max)); par(new=T)
  plot(time, minus, xlab="", ylab="", type = "l", col="green", xlim=c(x_min,x_max), ylim=c(y_min,y_max));
  legend("topright", legend=c("Mean+SD", "Mean", "Mean-SD"), col=c("blue", "red", "green"), lty=c(1,1,1))

  dev.off()


  file_name <- paste(img_path2, ".png", sep = as.character(i))
  png(file = file_name)
  
  time <- data$Time
  mean <- data$Mean
  real <- data$Real
  x_max <- max(time)
  x_min <- min(time)
  y_max <- max(c(mean,real))
  y_min <- min(c(mean,real))
  # x_max <- 3600
  # x_min <- 0
  # y_max <- 400
  # y_min <- 0

  plot(time, mean, xlab="Time", ylab="Density", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(y_min,y_max)); par(new=T)
  plot(time, real, xlab="", ylab="", type = "l", col="blue", xlim=c(x_min,x_max), ylim=c(y_min,y_max))
  legend("topright", legend=c("Mean", "Real"), col=c("red", "blue"), lty=c(1,1))

  dev.off()
}
