## シミュレーションの結果を描画（各モードの生起確率）
# ファイル読み込み
file_path <-  "./../result/mode/sctm_result_mode_"
img_path <- "./mode/mode_probability_"
link_num <- 136

for (i in 0:link_num){
  name <- paste(file_path, ".csv", sep = as.character(i));
  data <- read.csv(name, head=T);
  names(data) <- c("Time", "FF", "CC", "CF", "FC1", "FC2")

  # グラフの描画と保存
  img_name <- paste(img_path, ".png", sep = as.character(i))
  png(file = img_name)

  time <- data$Time
  ff <- data$FF
  cc <- data$CC
  cf <- data$CF
  fc1 <- data$FC1
  fc2 <- data$FC2
  x_max <- max(time)
  x_min <- min(time)

  plot(time, ff, xlab="Time", ylab="Probability", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(0,1.5)); par(new=T)
  plot(time, cc, xlab="", ylab="", type = "l", col="blue", xlim=c(x_min,x_max), ylim=c(0,1.5)); par(new=T)
  plot(time, cf, xlab="", ylab="", type = "l", col="green", xlim=c(x_min,x_max), ylim=c(0,1.5)); par(new=T)
  plot(time, fc1, xlab="", ylab="", type = "l", col="pink", xlim=c(x_min,x_max), ylim=c(0,1.5)); par(new=T)
  plot(time, fc2, xlab="", ylab="", type = "l", col="yellow", xlim=c(x_min,x_max), ylim=c(0,1.5));
  legend("topright", legend=c("FF", "CC", "CF", "FC1", "FC2"), col=c("red", "blue", "green", "pink", "yellow"), lty=c(1,1,1,1,1))

  dev.off()
}
