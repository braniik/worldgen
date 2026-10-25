{
  description = "worldgen: a from-scratch tile world generator in C";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { self, nixpkgs }:
    let
      lib = nixpkgs.lib;

      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = f: lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});

      worldgen =
        {
          lib,
          stdenv,
          pkg-config,
          sdl3,
          mode ? "release",
        }:
        stdenv.mkDerivation {
          pname = "worldgen";
          version = "0.3";

          src = self;

          nativeBuildInputs = [ pkg-config ];
          buildInputs = [ sdl3 ];

          makeFlags = [ "MODE=${mode}" ];

          doCheck = true;
          checkTarget = "check";

          hardeningDisable = lib.optional (mode == "debug") "fortify";

          installPhase = ''
            runHook preInstall
            install -Dm755 build/${mode}/worldgen $out/bin/worldgen
            runHook postInstall
          '';

          meta = {
            description = "A from-scratch tile world generator in C";
            homepage = "https://github.com/braniik/worldgen";
            license = lib.licenses.mit;
            mainProgram = "worldgen";
            platforms = lib.platforms.linux;
          };
        };
    in
    {
      packages = forAllSystems (pkgs: {
        default = pkgs.callPackage worldgen { };
        debug = pkgs.callPackage worldgen { mode = "debug"; };
      });

      checks = forAllSystems (pkgs: {
        gcc = pkgs.callPackage worldgen { };
        clang = pkgs.callPackage worldgen { stdenv = pkgs.clangStdenv; };
        asan = pkgs.callPackage worldgen { mode = "debug"; };
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          inputsFrom = [ (pkgs.callPackage worldgen { }) ];

          packages = with pkgs; [
            gdb
            clang-tools
            bear
          ];

          hardeningDisable = [ "fortify" ];
        };
      });
    };
}
