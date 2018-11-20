#ファイル読み込み
observe <- read.csv("./observer_location.csv", head=T);
observe$link1 = observe$link * 2;
observe$link2 = observe$link1 + 1;

data <- data.frame(time=1:13,real=1:13, sctm=1:13)

real_path <- "./10min/traffic_flow_";
sctm_path <- "./../result/traffic_volume/sctm_result_traffic_volume_"

for (i in 1:71){
  # 観測値を読み込む
  real_num <- observe$observe[i];
  real_name <- paste(real_path, as.character(real_num), ".csv", sep="");
  real_data <- read.csv(real_name, head=T);
  
  data$real <- real_data$Flow;
  data$time <- real_data$Time;

  # シミュレーション結果を読み込む
  sctm_name1 <- paste(sctm_path, as.character(observe$link1[i]), ".csv", sep="");
  sctm_name2 <- paste(sctm_path, as.character(observe$link2[i]), ".csv", sep="");
  sctm_data1 <- read.csv(sctm_name1, head=T);
  sctm_data2 <- read.csv(sctm_name2, head=T);

  #余分な列を削除
  sctm_data1 <- sctm_data1[c(1:13),];
  sctm_data2 <- sctm_data2[c(1:13),];

  data$sctm <- (sctm_data1$Flow + sctm_data2$Flow) / 2;

 # グラフの描画と保存
  path <- "./traffic_volume/result_traffic_volume_"

  file_num <- observe$observe[i];
  file_name <- paste(path, as.character(file_num), ".png", sep="");
  png(file = file_name)

  time <- data$time
  sctm <- data$sctm
  real <- data$real
  x_max <- max(time)
  x_min <- min(time)
  y_max <- max(c(sctm,real))
  y_min <- min(c(sctm,real))

  plot(time, sctm, xlab="Time", ylab="Flow", type = "l", col="red", xlim=c(x_min,x_max), ylim=c(y_min,y_max)); par(new=T)
  plot(time, real, xlab="", ylab="", type = "l", col="blue", xlim=c(x_min,x_max), ylim=c(y_min,y_max));
  legend("topright", legend=c("SCTM", "REAL"), col=c("blue", "red"), lty=c(1,1))

  dev.off()
}

