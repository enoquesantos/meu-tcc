#### Instruções para criar um projeto utilizando essa arquitetura
----
Este projeto contém apenas uma arquitetura de software para um projeto de uma aplicação desktop (linux somente - por enquanto) e mobile (android bem suportado e iOS parcialmente suportado).

Para usar esta arquitetura, faça um fork desse projeto, faça um clone do projeto forkado para a sua máquina e renomeie a pasta clonada e o arquivo tcc.pro para o nome do seu projeto, que deve ser o nome do seu app. Em seguida, importe o projeto no QtCreator. No entando faça:

1. Primeiramente, abra o arquivo *AndroidManifest.xml* no QtCreator e renomeie o Package name, de *org.qtproject.example* para o nome do pacote do seu app.
2. Para suportar push notification, crie o seu projeto do [firebase](https://console.firebase.google.com/?hl=pt-br&pli=1) 
   - Adicione o suporte a [push-notification](https://console.firebase.google.com/project/novo-projeto-do-firebase/notification?hl=pt-br) ao projeto, configure as opções e no final do wizard, exporte o arquivo de configuração *google-services.json* e salve na pasta android (substitua o existente - ele é apenas um exemplo).
3. Edite o arquivo build.gradle e renomeie o valor da propriedade *defaultConfig.applicationId* (*org.qtproject.example*) para o package name do seu app. Essa alteração é necessária para funcionamento do firebase.
4. Para o app Android, os arquivos que você deve customizar para configurar a identidade do seu app são:
   - Edite o arquivo *android/res/values/colors.xml* e defina as cores do app. Observe que a cor definida em *colorPrimary* é usado para cor de fundo do ícone nas notificações de push ou notificações local (enviadas pelo app sem ser via push).
   - Edite o arquivo *android/res/values/styles.xml* e configure as propriedades de estilo do app. Por padrão, o app está configurado para usar um pacote de estilo de compatibilidade com versões antigas do android, ele será baixado durante o build e empacotado no APK como um jar. Ele está definido no arquivo gradle (compile 'com.android.support:appcompat-v7:+'). Para que ele seja aplicado no app, é feito uma atribuição no construtor do *CustomActivity: QT_ANDROID_THEMES = new String[] {"Theme.AppCompat"};*. Para ver outras propriedades de estilo acesso [Styles and Themes](https://developer.android.com/guide/topics/ui/look-and-feel/themes.html).
5. Adicione os ícones do seu app em *android/res/drawable-(*)dpi*. O projeto vem com ícones nos tamanhos ideais, substitua pelos seu ícones usando os tamanhos dos ícones fornecidos. Você pode usar alguma ferramenta online tais como: [Android Asset Studio](https://romannurik.github.io/AndroidAssetStudio/icons-launcher.html) ou [Android icon studio] (https://jgilfelt.github.io/AndroidAssetStudio/icons-launcher.html). Veja dicas de como gerar ícones usando o [Android Studio](https://developer.android.com/studio/write/image-asset-studio.html?hl=pt-br).
6. Gere também os ícones com fundo transparente para usa na notificação, requerido para versões a partir do android Lollipop (API 21). Logo, para cada ícone do lançador presente em *android/res/drawable-(*)dpi* mantenha uma versão com fundo transparente. A cor será definida via java na classe que envia a notificação para a bandeija do sistema. Perceba que existe um ícone de nome *icon_transparent.png* em cada drawable. Mantenha este nome pois ele é referenciado nas classes do firebase e Notification.java. Para mais detalhes sobre essa questão do ícone transparente, leia essa discussão [neste link](https://stackoverflow.com/questions/30795431/icon-not-displaying-in-notification-white-square-shown-instead).
7. Na raiz do projeto, na pasta *images*, mantenha uma cópia dos ícones do app (versão transparente e normal). Adicione também a imagem do navigation drawer (menu lateral) substituindo as versões fornecidas como exemplo. A imagem *default_user_image.svg* é utilizada na imagem de perfil do usuário quando ela não tiver sido definida. Você também pode substituir essa imagem quando desejar.
8. Para adicionar uma imagem de splash, exibida no startup do app, adicione uma imagem com as resoluções recomendadas [neste link](http://bijudesigner.com/blog/app-icon-and-splash-screen-sizes/) em cada drawable (*android/res/drawable-(*)dpi*). Para que ela seja utilizada, basta descomentar as linhas 60 e 61 no arquivo *AndroidManifest.xml*. Porém, existe boas práticas e melhores implementações que vão além de configuração via xml para exibir uma tela de splash, por exemplo contido [neste link](https://android.jlelse.eu/right-way-to-create-splash-screen-on-android-e7f1709ba154) [ou aqui] (https://www.bignerdranch.com/blog/splash-screens-the-right-way/).
9. Escreva os seus plugins. O app não possui nenhuma funcionalidade de tela fornecida, você deve implementar nos plugins. Os plugins serão carregados e mapeados numa lista de objetos.


#### Requisitos para gerar um app android:
----
1. Qt Creator for Android Versão 5.9
2. Android NDK R14
3. Android SDK (instalar os seguintes pacotes):
   - Android SDK Build-Tools >= 26
   - Android SDK Platform-Tools >= 26
   - Android SDK Tools Versão >= 26
   - Android 5.1 (API 22 SDK Platform)
   - Android 6.0 (API 23 SDK Platform)
   - Android 7.1.1 (API 25 SDK Platform)
   - Android Support Repository REV 47
   - Google Play Licensing Library REV 1
   - Google Play Services REV 41
   - Google Repository REV 53


#### Instalando as dependências manualmente (sem android studio)
----
1. No diretório do SDK, acesse tools/bin e execute os comandos abaixo: (android extras é requerido pelo Firebase (Push Notification))
   - ./sdkmanager "tools"
   - ./sdkmanager "platform-tools"
   - ./sdkmanager "build-tools;26.0.2"
   - ./sdkmanager "platforms;android-22"
   - ./sdkmanager "platforms;android-23"
   - ./sdkmanager "platforms;android-24"
   - ./sdkmanager "platforms;android-25"
   - ./sdkmanager "extras;android;m2repository"
   - ./sdkmanager "extras;google;market_licensing"
   - ./sdkmanager "extras;google;google_play_services"
   - ./sdkmanager "extras;google;m2repository"


#### Observações sobre as dependências:
----
1. As APIs Level (System Image) é necessário quando testar o app em um emulador.
2. Caso queira testar o aplicativo em um emulador, Push Notification não funcionará.
3. O SDK mais recente não dispõe de um gerenciador gráfico para instalar os pacotes listados acima, execute os comandos abaixo.
4. Caso use o Android Studio, marque os checkbox correspondentes aos itens do Android SDK (item 3).
5. Para maiores informações, acesse:
   - https://developer.android.com/studio/command-line/sdkmanager.htm
   - http://doc.qt.io/qt-5/androidgs.html
