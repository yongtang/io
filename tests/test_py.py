import os
  
import tensorflow as tf
import tensorflow_io as tfio

def test_case():
  @tf.function
  def f(v):
    v = tfio.audio.decode_wav(v, dtype=tf.int16)
    v = tfio.audio.encode_wav(v, 44100)
    return v

  path = os.path.join(
      os.path.dirname(os.path.abspath(__file__)),
      "test_audio",
      "ZASFX_ADSR_no_sustain.wav",
  )
  content = tf.io.read_file(path)

  value = f(content)
