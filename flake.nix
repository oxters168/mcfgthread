{
  description = "mcfgthread - A mingw-w64 thread implementation";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem [ "aarch64-darwin" ] (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        
        # Windows targets
        mingwTargets = {
          x86 = {
            pkgs = pkgs.pkgsCross.mingw32;
            arch = "i686";
          };
          x86_64 = {
            pkgs = pkgs.pkgsCross.mingwW64;
            arch = "x86_64";
          };
        };

        # Common build function
        buildMcfgthread = target:
          target.pkgs.stdenv.mkDerivation {
            pname = "mcfgthread-${target.arch}";
            version = "1.0.0";
            
            src = ./.;
            
            nativeBuildInputs = with pkgs; [
              meson
              ninja
              pkg-config
            ];
            
            mesonFlags = [
              "--cross-file=${./cross-${target.arch}.txt}"
              "--buildtype=release"
              "--default-library=static"
            ];
          };

      in {
        packages = {
          x86 = buildMcfgthread mingwTargets.x86;
          x86_64 = buildMcfgthread mingwTargets.x86_64;
          default = pkgs.symlinkJoin {
            name = "mcfgthread-all";
            paths = with self.packages.${system}; [ x86 x86_64 ];
          };
        };
      }
    );
}
