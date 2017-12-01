#### Requisitos para gerar um app android:
================================================
1. Qt Creator for Android Versão 5.9
2. Android NDK R14
3. Android SDK (instalar os seguintes pacotes):
* Android SDK Build-Tools >= 26
* Android SDK Platform-Tools >= 26
* Android SDK Tools Versão >= 26
* Android 5.1 (API 22 SDK Platform)
* Android 6.0 (API 23 SDK Platform)
* Android 7.1.1 (API 25 SDK Platform)
* Android Support Repository REV 47
* Google Play Licensing Library REV 1
* Google Play Services REV 41
* Google Repository REV 53


#### Obs:
================================================
1. As APIs Level (System Image) é necessário quando testar o app em um emulador.
2. Caso queira testar o aplicativo em um emulador, Push Notification não funcionará.
3. O SDK mais recente não dispõe de um gerenciador gráfico para instalar os pacotes listados acima, execute os comandos abaixo.
4. Caso use o Android Studio, marque os checkbox correspondentes aos itens do Android SDK (item 3).
5. Para maiores informações, acesse:
    1. https://developer.android.com/studio/command-line/sdkmanager.html
    2. http://doc.qt.io/qt-5/androidgs.html


#### Instalando manualmente (sem android studio)
================================================
**No diretório do SDK, acesse tools/bin e execute os comandos abaixo: (android extras é requerido pelo Firebase (Push Notification))**
    1.  ./sdkmanager "tools"
    2.  ./sdkmanager "platform-tools"
    3.  ./sdkmanager "build-tools;26.0.2"
    4.  ./sdkmanager "platforms;android-22"
    5.  ./sdkmanager "platforms;android-23"
    6.  ./sdkmanager "platforms;android-24"
    7.  ./sdkmanager "platforms;android-25"
    8.  ./sdkmanager "extras;android;m2repository"
    9.  ./sdkmanager "extras;google;market_licensing"
    10. ./sdkmanager "extras;google;google_play_services"
    11. ./sdkmanager "extras;google;m2repository"
