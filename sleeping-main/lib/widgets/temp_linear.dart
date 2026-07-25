import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_sliders/sliders.dart';
import 'package:syncfusion_flutter_core/theme.dart';

class TempLinear extends StatelessWidget {
  const TempLinear({super.key, required this.value});

  final double value;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        SfSliderTheme(
            data: SfSliderThemeData(
              // overlayColor: Colors.red,
              thumbColor: Colors.red,
              activeTrackColor: Colors.red,
              inactiveTrackColor: Colors.red.withOpacity(0.5),
              overlayRadius: 0,
              thumbRadius: 8,
              activeTrackHeight: 8,
            ),
            child: SfSlider.vertical(
              interval: 20,
              showDividers: true,
              stepSize: 20,
              max: 100.0,
              value: value,
              onChanged: (dynamic values) {},
              enableTooltip: true,
              showLabels: true,
            ),
        ),
        const SizedBox(height: 18,),
        Text(
          'Temperature : $value °C',
          style: const TextStyle(
            fontSize: 20,
          ),
        ),
      ],
    );
  }
}
