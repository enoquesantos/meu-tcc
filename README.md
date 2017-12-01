### Requisitos para compilação do aplicativo no android:

1. Qt Creator for Android Versão 5.9

2. Android NDK R14

3. Android SDK (instalar os seguintes pacotes):
    3.1 Android SDK Tools Versão 25.2.5
    3.2 Android SDK Platform-Tools Versão 26
    3.3 Android SDK Build-Tools Versão 26
    3.4 Android 5.1.1 (API 22 SDK Platform)
    3.5 Android 6.0 (API 23 SDK Platform)
    3.6 Android 7.0 (API 24 SDK Platform)
    3.7 Android 7.1.1 (API 25 SDK Platform)
    3.8 Android Support Repository REV 47
    3.9 Google Play Licensing Library REV 1
    3.10 Google Play Services REV 41
    3.11 Google Repository REV 53

Obs:
As APIs Level (System Image) é necessário somente se for executar o app em um emulador.
Caso queira testar o aplicativo em um emulador, o Push Notification não funcionará.

##### O SDK mais recente não dispõe de um gerenciador gráfico para instalar os pacotes listados acima, execute os comandos abaixo.
##### Caso use o Android Studio, marque os checkbox correspondentes aos itens do Android SDK (item 3).
##### Para maiores informações, acesse:
1. https://developer.android.com/studio/command-line/sdkmanager.html
2. http://doc.qt.io/qt-5/androidgs.html

##### No diretório do SDK, acesse tools/bin e executar os comandos abaixo:
##### Android Extras são libs requerido pelo Firebase (Push Notification)
1.  ./sdkmanager "tools"
2.  ./sdkmanager "platform-tools"
3.  ./sdkmanager "build-tools;26.0.0"
4.  ./sdkmanager "platforms;android-22"
5.  ./sdkmanager "platforms;android-23"
6.  ./sdkmanager "platforms;android-24"
7.  ./sdkmanager "platforms;android-25"
8.  ./sdkmanager "extras;android;m2repository"
9.  ./sdkmanager "extras;google;market_licensing"
10. ./sdkmanager "extras;google;google_play_services"
11. ./sdkmanager "extras;google;m2repository"
