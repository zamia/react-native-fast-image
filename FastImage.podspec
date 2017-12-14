require 'json'

package = JSON.parse(File.read(File.join(__dir__, 'package.json')))

Pod::Spec.new do |s|

  s.name           = 'FastImage'
  s.version        = package['version'].gsub(/v|-beta/, '')
  s.summary        = package['description']
  s.author         = package['author']
  s.license        = package['license']
  s.homepage       = package['homepage']
  s.source         = { :git => 'https://github.com/DylanVann/react-native-fast-image.git', :tag => "v#{s.version}-beta"}
  s.ios.deployment_target = '8.0'
  #s.preserve_paths = '*.js'
  #s.library        = 'z'

  s.dependency 'React'
  s.dependency 'SDWebImage/WebP'

  s.source_files = 'ios/FastImage/*.{h,m}'
  s.dependency 'SDWebImage/GIF', '4.2.2'
end
