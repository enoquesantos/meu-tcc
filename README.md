#### Instruções para criar um projeto utilizando esta arquitetura:
Este projeto contém apenas uma simples arquitetura de software para um projeto de uma aplicação desktop (Linux plasma KDE bem suportado e OSX parcialmente suportado) e mobile (Android bem suportado e iOS parcialmente suportado).


----
#### Para usar esta arquitetura, faça:
1. Crie um fork desse projeto
2. Clone o projeto forkado para a sua máquina
3. Renomeie a pasta clonada e o arquivo *tcc.pro* para o nome do seu projeto (pode ser o nome do seu app)
4. Importe o projeto no *QtCreator* e siga os passos seguintes para configurar a customização do seu app.


----
#### Para customizar o seu aplicativo, faça:
1. Primeiramente, abra o arquivo *AndroidManifest.xml* no QtCreator e renomeie o *package name*, de *org.qtproject.example* para o nome do pacote do seu app.
2. Para suportar push notification, crie um projeto do [firebase](https://console.firebase.google.com)
   - Adicione o suporte a [push-notification](https://console.firebase.google.com/project/novo-projeto-do-firebase/notification) ao projeto, configure as opções e ao final do *wizard* de configuração, exporte o arquivo de configuração *google-services.json* e salve na pasta android (substitua o existente - ele é apenas um exemplo).
3. Edite o arquivo *build.gradle* e renomeie o valor da propriedade *defaultConfig.applicationId* de (*org.qtproject.example*) para o *package name* do seu app. Essa alteração é necessária para o funcionamento do firebase.
4. Para customizar a identidade do seu Android app, você deve editar os seguintes arquivos:
   - Edite o arquivo *android/res/values/colors.xml* e defina as cores do app. Observe que a cor definida em *colorPrimary* é usado para cor de fundo do ícone nas notificações de push ou local (enviadas pelo app sem ser via push).
   - Edite o arquivo *android/res/values/styles.xml* e configure as propriedades de estilo do app. Por padrão, o app está configurado para usar um pacote de estilo de compatibilidade com versões antigas do android, ele será baixado durante o build e empacotado no APK como um jar. Ele está definido no arquivo *build.gradle* na linha 34 (implementation 'com.android.support:appcompat-v7:+'). Para que ele seja aplicado no app, é feito uma atribuição no construtor do *CustomActivity: QT_ANDROID_THEMES = new String[] {"Theme.AppCompat"};*. Para ver outras opções e propriedades de estilo acesse a página [Styles and Themes](https://developer.android.com/guide/topics/ui/look-and-feel/themes.html).
5. Adicione os ícones do seu app em *android/res/drawable-(*)dpi*. O projeto vem com ícones nos tamanhos ideais, substitua pelos seu ícones usando os tamanhos dos ícones fornecidos. Você pode usar alguma ferramenta online para gerar os ícones, tais como: [Android Asset Studio](https://romannurik.github.io/AndroidAssetStudio/icons-launcher.html) ou [Android icon studio] (https://jgilfelt.github.io/AndroidAssetStudio/icons-launcher.html). Veja dicas de como gerar ícones usando o [Android Studio](https://developer.android.com/studio/write/image-asset-studio.html).
6. Gere também os ícones com fundo transparente para uso na notificação, requerido para versões a partir do Lollipop (API 21). Logo, para cada ícone do lançador presente em *android/res/drawable-(*)dpi* mantenha uma versão com fundo transparente. A cor de fundo será definida via java na classe que envia a notificação para a bandeija do sistema. Perceba que existe um ícone de nome *icon_transparent.png* em cada drawable. Mantenha este nome pois ele é referenciado nas classe Notification.java. Para mais detalhes sobre essa questão do ícone transparente, [leia essa discussão](https://stackoverflow.com/questions/30795431/icon-not-displaying-in-notification-white-square-shown-instead).
7. Na raiz do projeto, na pasta *images*, mantenha uma cópia dos ícones do app (versão transparente e normal). Adicione também a imagem do navigation drawer (menu lateral) substituindo as versões fornecidas como exemplo. A imagem *default_user_image.svg* é utilizada na imagem de perfil do usuário quando ela não tiver sido definida. Você também pode substituir essa imagem por uma que achar melhor, mas mantenha o mesmo nome do arquivo.
8. Para adicionar uma imagem de splash, exibida na inicialização do app, adicione uma imagem com as resoluções recomendadas [neste link](http://bijudesigner.com/blog/app-icon-and-splash-screen-sizes/) em cada drawable (*android/res/drawable-(*)dpi*). Para que ela seja utilizada, basta descomentar as linhas 60 e 61 no arquivo *AndroidManifest.xml*. Porém, existe boas práticas e melhores implementações que vão além de configuração via xml para exibir uma tela de splash, por exemplo publicado [neste post](https://android.jlelse.eu/right-way-to-create-splash-screen-on-android-e7f1709ba154) [ou aqui] (https://www.bignerdranch.com/blog/splash-screens-the-right-way/).
9. Escreva os seus plugins. O seu app deve fornecer as funcionalidades através de plugins que você pode implementar a partir dos exemplos fornecidos. Os plugins não são mapeados no arquivo qrc e você deve criá-los manualmente matendo-os em uma pasta com o nome do seu plugin. Não utilize o wizard de adição de arquivos do QtCreator, pois ele esse novo arquivo será adicionado ao qml.qrc, que contém os componentes reutilizáveis do projeto e isso violará um requisito desta arquitetura. Os plugins não devem ser mapeados em arquivos qrc pois são independentes entre sí e serão conhecidos e carregados dinamicamente. Uma lista de objetos contendo todos os arquivos dos plugins pode ser acessado por qualquer componente.


----
#### Requisitos para gerar um app android:
1. Qt Creator for Android Versão 5.9. [Baixe aqui](http://download.qt.io/official_releases/qt/5.9/5.9.3/qt-opensource-linux-x64-5.9.3.run)
2. Android NDK R14 ou superior.
   - No caso do NDK, é necessário apenas baixar o pacote, descompactá-lo e informar o path da pasta nas configurações do QtCreator.
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
4. Java JDK localmente instalado.
   - [Veja como instalar no Debian 9](http://www.itzgeek.com/how-tos/linux/debian/how-to-install-oracle-java-8-on-debian-9-ubuntu-linux-mint.html)
   - [Veja como instalar no Arch Linux](https://www.ostechnix.com/install-oracle-java-8-arch-linux/)
5. Gradle versão 4.3.1 ou acima localmente instalado. Esse projeto está confogurado para usar a versão mais recente do Gradle.
   - [Veja como instalar no seu sistema](https://gradle.org/install/). O Gradle está nos repositórios das prinicipais distribuições Linux.
   - Leia as opção de build desta versão [aqui](https://developer.android.com/studio/build/gradle-plugin-3-0-0-migration.html#new_configurations).
   - No primeiro build, o gradle vai baixar algumas libs requeridas para compilação do projeto e salvar na home do usuário. Do segundo build em diante ele usará as as libs salvas localmente, tal como as libs do *Firebase*.
6. Adicione os paths do JDK, Android SDK e NDK nas configurações do QtCreator. Para isso, faça:
   - Clique no menu Tools -> Options -> Devices -> Android e informe o caminho absoluto dos kits e clique em Ok.


----
#### Instalando as dependências manualmente (sem android studio):
1. No diretório do SDK, acesse tools/bin e execute os comandos abaixo:
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

   - **android extras é requerido pelo *Firebase para o Push Notification***


----
#### Observações sobre as dependências:
1. As APIs Level (System Image) é necessário quando testar o app em um emulador.
2. Caso queira testar o aplicativo em um emulador, *Push Notification* não funcionará.
3. Links úteis:
   - http://doc.qt.io/qt-5/androidgs.html
   - https://developer.android.com/studio/command-line/sdkmanager.htm
