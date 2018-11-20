# ファイル読み込み
file_path <-  "./../result/flow/sctm_result_inflow_outflow_"
img_file <- "./flow/in-outflow_"

for (i in 0:7){
  name <- paste(file_path, ".csv", sep = as.character(i));
  data <- read.csv(name, head=T);
  names(data) <- c("Time", "In", "Out")

  img_name <- paste(img_file, ".png", sep = as.character(i))
  png(file = img_name)
  
  time <- data$Time
  inflow <- data$In
  outflow <- data$Out
  x_max <- max(time)
  x_min <- min(time)
  y_max <- max(c(inflow,outflow))
  y_min <- min(c(inflow,outflow))
  # x_max <- 3600
  # x_min <- 0
  # y_max <- 400
  # y_min <- 0

  plot(time, inflow, xlab="Time", ylab="Flow", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(y_min,y_max)); par(new=T);
  plot(time, outflow, xlab="", ylab="", type = "l", col="blue", xlim=c(x_min,x_max), ylim=c(y_min,y_max))
  legend("topright", legend=c("IN", "OUT"), col=c("red", "blue"), lty=c(1,1))

  dev.off()
}
