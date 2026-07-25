import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_gauges/gauges.dart';

class RadialChar extends StatelessWidget {
  const RadialChar({super.key, this.maxRange, required this.value, required this.title});

  final double? maxRange;
  final double value;
  final String title;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        SfRadialGauge(
          enableLoadingAnimation: true,
          animationDuration: 2500,
          axes: <RadialAxis>[
            RadialAxis(
              showLabels: false,
              showTicks: false,
              radiusFactor: 0.8,
              maximum: maxRange ?? 240,
              axisLineStyle: const AxisLineStyle(
                cornerStyle: CornerStyle.startCurve,
                thickness: 5,
              ),
              annotations: <GaugeAnnotation>[
                GaugeAnnotation(
                  angle: 90,
                  widget: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: <Widget>[
                      Text(
                        value.toString(),
                        style: const TextStyle(
                          fontWeight: FontWeight.bold,
                          fontStyle: FontStyle.italic,
                          fontSize: 30,
                        ),
                      ),
                      // const Padding(
                      //   padding: EdgeInsets.fromLTRB(0, 2, 0, 0),
                      //   child: Text(
                      //     'km/h',
                      //     style: TextStyle(
                      //       fontWeight: FontWeight.bold,
                      //       fontStyle: FontStyle.italic,
                      //       fontSize: 14,
                      //     ),
                      //   ),
                      // )
                    ],
                  ),
                ),
              ],
              pointers: <GaugePointer>[
                RangePointer(
                  value: value,
                  width: 18,
                  pointerOffset: -6,
                  cornerStyle: CornerStyle.bothCurve,
                  color: const Color(0xFFF67280),
                  gradient: const SweepGradient(
                    colors: <Color>[Color(0xFFFF7676), Color(0xFFF54EA2)],
                    stops: <double>[0.25, 0.75],
                  ),
                ),
                MarkerPointer(
                  value: value,
                  color: Colors.white,
                  markerType: MarkerType.diamond,
                ),
              ],
            ),
          ],
        ),
        Text(
          title,
          style: const TextStyle(
            fontSize: 18,
          ),
        ),
      ],
    );
  }
}
