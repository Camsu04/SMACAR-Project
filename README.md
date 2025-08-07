
# SMACAR-Project

**Sistema IoT solar + LoRa para monitoreo de agua en zonas rurales**

Monitorea pH, turbidez, TDS y transmite los datos mediante LoRa. ¡Incluye app móvil, hardware impreso en 3D y visualización de datos!

---

## 🌟 Vista rápida

### Diseño del sistema

![Diagrama general](Hardware/Electronic/SMACAR%20MAIN%20BOARD/Diagrama.png)


---

### Prototipo físico armado

![Prototipo físico](Documentacion/PROYECTO FINAL.jpg)

---

### Carcasa 3D y electrónica

| Carcasa ensamblada                       | Electrónica interna                        |
|-------------------------------------------|--------------------------------------------|
| ![Carcasa 3D](Documentacion/CARCASA.jpg) | ![PCB y conexiones](Documentacion/ELECTRONICA.jpg) |

---

### Diseño 3D animado

![Animación carcasa 3D](Hardware/3D%20Design/Carcasa%20LoRaWAN/ensamble%20gif.gif)
![Animación carcasa 3D](Hardware/3D%20Design/Carcasa%20placa%20Trasmisora/ensamblaje%20gif.gif)

---

### App móvil

| Pantalla principal | App en funcionamiento |
|--------------------|----------------------|
| ![App móvil](mobile%20app/APP.jpg) | ![App funcionando](Software/mobile%20app/APP_MOBILE_EN_FUNCIONAMIENTO.jpg) |


---

## 📁 Estructura del repositorio

```
SMACAR-Project/
├── Documentacion/           # Diagramas, fotos y GIFs de diseño
│   ├── Animacion_Carcasa.gif
│   ├── CARCASA.jpg
│   ├── DIAGRAMA.png
│   ├── ELECTRONICA.jpg
│   ├── PROYECTO FINAL.jpg
├── Hardware/                # Archivos y esquemas electrónicos
├── Software firmware/       # Código fuente para ESP32 y LoRa
├── mobile app/              # Capturas de la app móvil
│   ├── APP.jpg
│   ├── APP MOBILE EN FUNCIONAMIENTO.jpg
├── LICENSE
└── README.md
```

---

## 🚀 ¿Cómo empezar?

1. **Clona el repositorio:**
    ```bash
    git clone https://github.com/Camsu04/SMACAR-Project.git
    cd SMACAR-Project
    ```

2. **Revisa los esquemas y fotos** en la carpeta `Documentacion/`.

3. **Carga el firmware** desde `Software firmware/` a tu ESP32 usando [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html).

4. **Configura los sensores** (pH, turbidez, TDS) según el diseño mostrado y alimenta el sistema (idealmente con energía solar).

5. **Visualiza los datos** usando la app móvil incluida.

---

## ⚡ Características

- Sistema modular, autónomo y robusto
- Monitoreo de agua en tiempo real
- Notificaciones y visualización desde tu móvil
- Carcasa resistente impresa en 3D
- Bajo consumo y operación solar
- Fácil de instalar y ampliar

---

## 🛠️ Requisitos

- **ESP32**
- Sensores de pH, turbidez y TDS
- Módulo LoRa (compatible ESP32)
- Fuente solar o alimentación adecuada
- Teléfono Android para la app móvil

---

## 📲 App móvil

Encuentra las capturas y funcionamiento en la carpeta `mobile app/`.
La app permite visualizar datos de los sensores, recibir alertas y consultar el historial.

---

## 🤝 Contribuciones

¿Tienes ideas o mejoras?
- Abre un *Issue* para dudas o bugs
- Haz un *Pull Request* si mejoras el código, documentación o la app móvil

---

## 📄 Licencia

Proyecto bajo licencia MIT.
Consulta el archivo [LICENSE](LICENSE).

---

## 👨‍💻 Autores

- [Camsu04](https://github.com/Camsu04)
- [Fausto123f](https://github.com/Fausto123f)
- [j0nfut](https://github.com/j0nfut)
- [RiQ4](https://github.com/RiQ4)
