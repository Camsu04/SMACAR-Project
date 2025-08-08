
# SMACAR-Project 
![LOGO](Documentacion/logo.png)

**SMACAR: SISTEMA DE MONITOREO Y ALERTA DE CALIDAD DEL AGUA RURAL**

Plataforma distribuida de monitoreo de calidad de agua diseñada para entornos rurales: cada nodo utiliza un ESP32 programado íntegramente en C que adquiere parámetros críticos (pH, turbidez, conductividad y temperatura) mediante sensores industriales. La comunicación se realiza vía LoRaWAN, asegurando transmisión de datos robusta, segura y de largo alcance hacia una interfaz web/móvil de acceso remoto. El sistema opera con energía renovable, priorizando autonomía y bajo mantenimiento. El diseño prioriza robustez, escalabilidad y validación científica, permitiendo despliegue inmediato, integración con infraestructuras existentes y generación automatizada de alertas para respuesta temprana ante riesgos sanitarios.

---

## 🌟 Vista rápida

### Diseño del sistema

![Diagrama general](Hardware/Electronic/SMACAR%20MAIN%20BOARD/Diagrama1.png)
![Diagrama general](Hardware/Electronic/SMACAR%20RECEIVER%20BOARD/Diagrama.png)


---

### Prototipo físico armado

Coming soon...
// ![Prototipo físico](Documentacion/PROYECTO FINAL.jpg)

---

### Electronica
Coming soon...
| PCB TRANSMISOR                            | PCB RECEPTOR                               |
|-------------------------------------------|--------------------------------------------|
| ![PCB MAIN](Hardware/Electronic/SMACAR%20MAIN%20BOARD/RENDER.png) | ![PCB RECEPTOR](Hardware/Electronic/SMACAR%20RECEIVER%20BOARD/RENDER.png) |

---

### Carcasa 3D 

| Transmisor y sensores                     | RECEPTOR                                   |
|-------------------------------------------|--------------------------------------------|
| ![Carcasa 3D](Documentacion/CARCASA.jpg) | ![Carcasa](Documentacion/receptor.jpg) |

---

### Diseño 3D animado

![Animación carcasa 3D](Hardware/3D%20Design/Carcasa%20LoRaWAN/ensamble%20gif.gif)
![Animación carcasa 3D](Hardware/3D%20Design/Carcasa%20placa%20Trasmisora/ensamblaje%20gif.gif)

---

### App móvil

| Pantalla principal | App en funcionamiento |
|--------------------|----------------------|
| ![App móvil](Software/mobile%20app/APP_fuera.jpg) | ![App funcionando](Software/mobile%20app/APP_MOBILE_EN_FUNCIONAMIENTO.jpg) |

### DASHBOARD WEB

![Dashboard funcionando](Software/dashboard/DASHBOARD%20BLYNK%202.png)


---

## 📁 Estructura del repositorio

```
SMACAR-Project/
├── Documentacion/           # Diagramas, fotos y GIFs de diseño
│   ├── Layout.png
│   ├── CARCASA.jpg
│   ├── DIAGRAMA.pdf
│   ├── DATASHEET.pdf
│   ├── PROYECTO FINAL.jpg
├── Hardware/                # Archivos y esquemas electrónicos + Diseños 3d en solidworks
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
