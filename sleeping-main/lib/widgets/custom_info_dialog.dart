import 'package:flutter/material.dart';

class CustomInfoDialog extends StatelessWidget {
  const CustomInfoDialog({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      mainAxisSize: MainAxisSize.min,
      children: [
        buildTextSpan(
          title: 'Sleep apnea: ',
          content:
          'is a sleep disorder where breathing repeatedly stops and starts during sleep, leading to poor sleep quality and potential health risks.',
        ),
        const SizedBox(height: 10),
        const Text(
          'Types:',
          style: TextStyle(
            fontWeight: FontWeight.bold,
            fontSize: 18,
          ),
        ),
        buildTextSpan(
          title: '1-Obstructive Sleep Apnea (OSA): ',
          content:
          'The most common type, caused by a physical blockage in the airway.',
        ),
        buildTextSpan(
          title: '2-Central Sleep Apnea: ',
          content:
          'Occurs when the brain fails to send proper signals to the muscles that control breathing.',
        ),
        buildTextSpan(
          title: '3-Complex Sleep Apnea: ',
          content:
          'A combination of OSA and central sleep apnea.',
        ),
        const SizedBox(height: 10),
        const Text(
          'Symptoms:',
          style: TextStyle(
            fontWeight: FontWeight.bold,
            fontSize: 18,
          ),
        ),
        const Text(
          '''
1- Loud snoring.
2- Pauses in breathing during sleep
3- Gasping or choking for air
4- Excessive daytime sleepiness
5- Dry mouth or sore throat upon waking
6- Morning headaches and irritability
                          ''',
        ),
        const SizedBox(height: 10),
        const Text(
          'Risk Factors:',
          style: TextStyle(
            fontWeight: FontWeight.bold,
            fontSize: 18,
          ),
        ),
        const Text(
          '''
1- Obesity
2- Age (more common in older adults)
3- Gender (more common in men)
4- Family history
5- Smoking, alcohol, and sedative use
                          ''',
        ),
        const SizedBox(height: 10),
        const Text(
          'Diagnosis:',
          style: TextStyle(
            fontWeight: FontWeight.bold,
            fontSize: 18,
          ),
        ),
        const Text(
          'A sleep study (polysomnography) is the most common method to diagnose sleep apnea.',
        ),
      ],
    );
  }
  
  buildTextSpan({required String title, required String content}) {
    return Text.rich(
      TextSpan(
        children: [
          TextSpan(
            text: title,
            style: const TextStyle(
              fontWeight: FontWeight.bold,
              fontSize: 18,
            ),
          ),
          TextSpan(
            text: content,
            style: const TextStyle(
              fontSize: 18,
            ),
          ),
        ],
      ),
    );
  }
}
